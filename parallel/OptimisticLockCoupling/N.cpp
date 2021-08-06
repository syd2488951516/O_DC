#include <assert.h>
#include <algorithm>

#include "N.h"
#include "N4.cpp"
#include "N16.cpp"
#include "N48.cpp"
#include "N256.cpp"



namespace ART_OLC {

    void N::setType(NTypes type) { // 类型和 typeVersionLockObsolete 关联
        typeVersionLockObsolete.fetch_add(convertTypeToVersion(type));
    }

    uint64_t N::convertTypeToVersion(NTypes type) {
        return (static_cast<uint64_t>(type) << 61);
    }

    NTypes N::getType() const {
        return static_cast<NTypes>(typeVersionLockObsolete.load(std::memory_order_relaxed) >> 61);
    }

    void N::writeLockOrRestart(bool &needRestart) {

        uint64_t version;
        version = readLockOrRestart(needRestart);
        if (needRestart) return;

        upgradeToWriteLockOrRestart(version, needRestart);
        if (needRestart) return;
    }
    
    void N::upgradeToWriteLockOrRestart(uint64_t &version, bool &needRestart) {
        //原子的比较typeVersionLockObsolete与version对比，相等的话替换
        if (typeVersionLockObsolete.compare_exchange_strong(version, version + 0b10)) {
            version = version + 0b10;
        } else {
            needRestart = true;//否则的话重启
        }
    }

    void N::writeUnlock() {
        //将typeVersionLockObsole与 fetch_add(0b100)相加，原子的替换原值
        typeVersionLockObsolete.fetch_add(0b10);
    }

    N *N::getAnyChild(const N *node) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<const N4 *>(node);
                return n->getAnyChild();
            }
            case NTypes::N16: {
                auto n = static_cast<const N16 *>(node);
                return n->getAnyChild();
            }
            case NTypes::N48: {
                auto n = static_cast<const N48 *>(node);
                return n->getAnyChild();
            }
            case NTypes::N256: {
                auto n = static_cast<const N256 *>(node);
                return n->getAnyChild();
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    bool N::change(N *node, uint8_t key, N *val) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                return n->change(key, val);
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                return n->change(key, val);
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                return n->change(key, val);
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                return n->change(key, val);
            }
        }
        assert(false);
        __builtin_unreachable();

    }
    template<typename curN>
    N* N::getParent(curN* node){
        if(node->parent){
            return node->parent;
        }
        return NULL;
    }

    N *N::minimum(N* node) {
        // Find the leaf with smallest key
        if (!node)
            return NULL;

        if (N::isLeaf(node)){
            //cout<<"is Leaf"<<endl;
            //cout<<getLeafValue(node)<<endl;
            return node;
        }
        switch (node->getType()) {
            case NTypes::N4: {
                //cout<<"sn sn 1"<<endl;
                auto n = static_cast<N4 *>(node);
                return minimum(n->children[0]);
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                return minimum(n->children[0]);
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                unsigned pos=0;
                while (n->childIndex[pos] == n->emptyMarker)
                    pos++;
                return minimum(n->children[n->childIndex[pos]]);
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                unsigned pos=0;
                while (!n->children[pos])
                    pos++;
                return minimum(n->children[pos]);
            }
        }
        throw; // Unreachable
    }

    N *N::maximum(N* node) {
        // Find the leaf with largest key
        if (!node)
            return NULL;

        if (N::isLeaf(node))
            return node;

        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                return maximum(n->children[n->count-1]);
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                return maximum(n->children[n->count-1]);
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                unsigned pos=255;
                while (n->childIndex[pos]== n->emptyMarker)
                    pos--;
                return maximum(n->children[n->childIndex[pos]]);
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                unsigned pos=255;
                while (!n->children[pos])
                    pos--;
                return maximum(n->children[pos]);
            }
        }
        throw; // Unreachable
    }


    

    //这个函数很重要（同一时刻有2个节点上锁）
    template<typename curN, typename biggerN>
    void N::insertGrow(curN *n, uint64_t v, N *parentNode, uint64_t parentVersion, uint8_t keyParent, uint8_t key, N *val, bool &needRestart, ThreadInfo &threadInfo) {
        if (!n->isFull()) {//如果当前节点未满
            if (parentNode != nullptr) {//父亲节点不为空的话，先对父亲节点解锁（验证）
                parentNode->readUnlockOrRestart(parentVersion, needRestart);
                if (needRestart) return;
            }
            //将当前节点的锁升级为写锁
            n->upgradeToWriteLockOrRestart(v, needRestart);
            if (needRestart) return;
            n->insert(key, val);//插入节点
            n->writeUnlock();//写锁解锁
            return;
        }
        //需要升级节点的情况（即当前节点要变大）
        //升级父亲节点的锁
        parentNode->upgradeToWriteLockOrRestart(parentVersion, needRestart);
        if (needRestart) return;
        //升级当前节点的锁
        n->upgradeToWriteLockOrRestart(v, needRestart);
        if (needRestart) {//需要重启的话，就是当前节点加锁失败，需要对父亲节点解写锁
            parentNode->writeUnlock();
            return;
        }
        //printf("change\n");
        auto nBig = new biggerN(n->getPrefix(), n->getPrefixLength());
        n->copyTo(nBig);
        nBig->insert(key, val);

        N::change(parentNode, keyParent, nBig);
        
        //当前节点解锁
        n->writeUnlockObsolete();
        //这个目前不太明白是什么意思，可能是记录一下线程的加解锁信息？有待验证
        threadInfo.getEpoche().markNodeForDeletion(n, threadInfo);//？？？？
        //父亲节点解锁
        parentNode->writeUnlock();
    }

    void N::insertAndUnlock(N *node, uint64_t v, N *parentNode, uint64_t parentVersion, uint8_t keyParent, uint8_t key, N *val, bool &needRestart, ThreadInfo &threadInfo) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                insertGrow<N4, N16>(n, v, parentNode, parentVersion, keyParent, key, val, needRestart, threadInfo);
                break;
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                insertGrow<N16, N48>(n, v, parentNode, parentVersion, keyParent, key, val, needRestart, threadInfo);
                break;
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                insertGrow<N48, N256>(n, v, parentNode, parentVersion, keyParent, key, val, needRestart, threadInfo);
                break;
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                insertGrow<N256, N256>(n, v, parentNode, parentVersion, keyParent, key, val, needRestart, threadInfo);
                break;
            }
        }
    }




    inline N *N::getChild(const uint8_t k, const N *node) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<const N4 *>(node);
                return n->getChild(k);
            }
            case NTypes::N16: {
                auto n = static_cast<const N16 *>(node);
                return n->getChild(k);
            }
            case NTypes::N48: {
                auto n = static_cast<const N48 *>(node);
                return n->getChild(k);
            }
            case NTypes::N256: {
                auto n = static_cast<const N256 *>(node);
                return n->getChild(k);
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    inline N *N::getMinChild(N *node) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<const N4 *>(node);
                return n->getMinChild();
            }
            case NTypes::N16: {
                auto n = static_cast<const N16 *>(node);
                return n->getMinChild();
            }
            case NTypes::N48: {
                auto n = static_cast<const N48 *>(node);
                return n->getMinChild();
            }
            case NTypes::N256: {
                auto n = static_cast<const N256 *>(node);
                return n->getMinChild();
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    inline N *N::getMaxChild(N *node) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<const N4 *>(node);
                return n->getMaxChild();
            }
            case NTypes::N16: {
                auto n = static_cast<const N16 *>(node);
                return n->getMaxChild();
            }
            case NTypes::N48: {
                auto n = static_cast<const N48 *>(node);
                return n->getMaxChild();
            }
            case NTypes::N256: {
                auto n = static_cast<const N256 *>(node);
                return n->getMaxChild();
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    void N::deleteChildren(N *node) {
        // if (N::isLeaf(node)) {
        //     return;
        // }
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                n->deleteChildren();
                return;
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                n->deleteChildren();
                return;
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                n->deleteChildren();
                return;
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                n->deleteChildren();
                return;
            }
            case NTypes::Nleaf: {
                //std::cout<<"dsds"<<std::endl;
                auto n = static_cast<LeafNode *>(node);
                //cout<<n->getLeafValue()<<endl;
                delete n;
                return;

            }
        }
        assert(false);
        __builtin_unreachable();
    }


    template<typename curN, typename smallerN>
    void N::removeAndShrink(curN *n, uint64_t v, N *parentNode, uint64_t parentVersion, uint8_t keyParent, uint8_t key, bool &needRestart, ThreadInfo &threadInfo) {
        if (!n->isUnderfull() || parentNode == nullptr) {
            if (parentNode != nullptr) {
                parentNode->readUnlockOrRestart(parentVersion, needRestart);
                if (needRestart) return;
            }
            n->upgradeToWriteLockOrRestart(v, needRestart);
            if (needRestart) return;

            n->remove(key);
            n->writeUnlock();
            return;
        }
        parentNode->upgradeToWriteLockOrRestart(parentVersion, needRestart);
        if (needRestart) return;

        n->upgradeToWriteLockOrRestart(v, needRestart);
        if (needRestart) {
            parentNode->writeUnlock();
            return;
        }

        auto nSmall = new smallerN(n->getPrefix(), n->getPrefixLength());

        //n->copyTo(nSmall); //add 注释
        nSmall->remove(key);
        N::change(parentNode, keyParent, nSmall);

        n->writeUnlockObsolete();
        threadInfo.getEpoche().markNodeForDeletion(n, threadInfo);
        parentNode->writeUnlock();
    }

    void N::removeAndUnlock(N *node, uint64_t v, uint8_t key, N *parentNode, uint64_t parentVersion, uint8_t keyParent, bool &needRestart, ThreadInfo &threadInfo) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                removeAndShrink<N4, N4>(n, v, parentNode, parentVersion, keyParent, key, needRestart, threadInfo);
                break;
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                removeAndShrink<N16, N4>(n, v, parentNode, parentVersion, keyParent, key, needRestart, threadInfo);
                break;
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                removeAndShrink<N48, N16>(n, v, parentNode, parentVersion, keyParent, key, needRestart, threadInfo);
                break;
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                removeAndShrink<N256, N48>(n, v, parentNode, parentVersion, keyParent, key, needRestart, threadInfo);
                break;
            }
        }
    }

    bool N::isLocked(uint64_t version) const {
        return ((version & 0b10) == 0b10);
    }

    uint64_t N::readLockOrRestart(bool &needRestart) const {
        uint64_t version;
        //std::atomic值的读取 用load()方法，写入用 store()方法
        version = typeVersionLockObsolete.load();
/*        do {
            version = typeVersionLockObsolete.load();
        } while (isLocked(version));*/
        //如果节点加锁或者节点已过时，则需要重启
        if (isLocked(version) || isObsolete(version)) {
            needRestart = true;
        }
        //否则返回版本号
        return version;
        //uint64_t version;
        //while (isLocked(version)) _mm_pause();
        //return version;
    }

    bool N::isObsolete(uint64_t version) {//判断最后1位，最后一位为1的时候返回true
        return (version & 1) == 1;
    }

    void N::checkOrRestart(uint64_t startRead, bool &needRestart) const {//调用的读锁解锁操作
        readUnlockOrRestart(startRead, needRestart);
    }

    void N::readUnlockOrRestart(uint64_t startRead, bool &needRestart) const {//验证版本
        needRestart = (startRead != typeVersionLockObsolete.load());
    }

    uint32_t N::getPrefixLength() const {
        return prefixCount;
    }

    bool N::hasPrefix() const {
        return prefixCount > 0;
    }

    uint32_t N::getCount() const {
        return count;
    }

    const uint8_t *N::getPrefix() const {
        return prefix;
    }

    void N::setPrefix(const uint8_t *prefix, uint32_t length) {
        if (length > 0) {
            memcpy(this->prefix, prefix, std::min(length, maxStoredPrefixLength));
            prefixCount = length;
        } else {
            prefixCount = 0;
        }
    }

    void N::addPrefixBefore(N *node, uint8_t key) {
        uint32_t prefixCopyCount = std::min(maxStoredPrefixLength, node->getPrefixLength() + 1);
        memmove(this->prefix + prefixCopyCount, this->prefix,
                std::min(this->getPrefixLength(), maxStoredPrefixLength - prefixCopyCount));
        memcpy(this->prefix, node->prefix, std::min(prefixCopyCount, node->getPrefixLength()));
        if (node->getPrefixLength() < maxStoredPrefixLength) {
            this->prefix[prefixCopyCount - 1] = key;
        }
        this->prefixCount += node->getPrefixLength() + 1;
    }


    bool N::isLeaf(const N *n) {
        return (reinterpret_cast<uint64_t>(n) & (static_cast<uint64_t>(1) << 63)) == (static_cast<uint64_t>(1) << 63);
    }

    N *N::setLeaf(TID tid) {
        return reinterpret_cast<N *>(tid | (static_cast<uint64_t>(1) << 63));
    }

    TID N::getLeaf(const N *n) {
        return (reinterpret_cast<uint64_t>(n) & ((static_cast<uint64_t>(1) << 63) - 1));
    }

    std::tuple<N *, uint8_t> N::getSecondChild(N *node, const uint8_t key) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                return n->getSecondChild(key);
            }
            default: {
                assert(false);
                __builtin_unreachable();
            }
        }
    }

    void N::deleteNode(N *node) {
        // if (N::isLeaf(node)) {
        //     return;
        // }
        if(node->getType() == NTypes::Nleaf){
            //auto n = static_cast<LeafNode *>(node);
            //cout<<n->getLeafValue()<<endl;
            //delete n;
            return;
        }
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                delete n;
                return;
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                delete n;
                return;
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                delete n;
                return;
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                delete n;
                return;
            }
        }
        delete node;
    }


    TID N::getAnyChildTid(const N *n, bool &needRestart) {
        //cout<<"getAnyCHildTid"<<endl;
        const N *nextNode = n;

        while (true) {
            const N *node = nextNode;
            auto v = node->readLockOrRestart(needRestart);
            if (needRestart) return 0;

            nextNode = getAnyChild(node);
            node->readUnlockOrRestart(v, needRestart);
            if (needRestart) return 0;

            assert(nextNode != nullptr);

            if (isLeaf(nextNode)) {
                return getLeaf(nextNode);
            }
        }
    }

    uint64_t N::getChildren(const N *node, uint8_t start, uint8_t end, std::tuple<uint8_t, N *> children[],
                        uint32_t &childrenCount) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<const N4 *>(node);
                return n->getChildren(start, end, children, childrenCount);
            }
            case NTypes::N16: {
                auto n = static_cast<const N16 *>(node);
                return n->getChildren(start, end, children, childrenCount);
            }
            case NTypes::N48: {
                auto n = static_cast<const N48 *>(node);
                return n->getChildren(start, end, children, childrenCount);
            }
            case NTypes::N256: {
                auto n = static_cast<const N256 *>(node);
                return n->getChildren(start, end, children, childrenCount);
            }
        }
        assert(false);
        __builtin_unreachable();
    }
}
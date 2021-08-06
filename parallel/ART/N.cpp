#include <assert.h>
#include <algorithm>

#include "N.h"
#include "N4.cpp"
#include "N16.cpp"
#include "N48.cpp"
#include "N256.cpp"




using namespace std;

namespace ART_unsynchronized {

    void N::setType(NTypes type) {
        this->type = type;
    }

    NTypes N::getType() const {
        return type;
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

    void N::change(N *node, uint8_t key, N *val) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                n->change(key, val);
                return;
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                n->change(key, val);
                return;
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                n->change(key, val);
                return;
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                n->change(key, val);
                return;
            }
        }
        assert(false);
        __builtin_unreachable();
    }


    template<typename curN, typename biggerN>//模板 N4->N16 N16->N48 N48->N256 写成一个统一的接口
    void N::insertGrow(curN *n, N *parentNode, uint8_t keyParent, uint8_t key, N *val) {
        if (n->insert(key, val)) {//如果curN，即当前节点未满，直接插入
            return;
        }
        //否则，升级节点（让节点变大）
        auto nBig = new biggerN(n->getPrefix(), n->getPrefixLength());
        n->copyTo(nBig);
        nBig->insert(key, val);

        N::change(parentNode, keyParent, nBig);

        delete n;
    }

    void N::insertA(N *node, N *parentNode, uint8_t keyParent, uint8_t key, N *val) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                insertGrow<N4, N16>(n, parentNode, keyParent, key, val);
                return;
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                insertGrow<N16, N48>(n, parentNode, keyParent, key, val);
                return;
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                insertGrow<N48, N256>(n, parentNode, keyParent, key, val);
                return;
            }
            case NTypes::N256: {
                //为了调用N256的插入方法，需要转化节点类型
                auto n = static_cast<N256 *>(node);
                n->insert(key, val);
                //cout<<"N-256 insert"<<endl;
                return;
            }
        }
        assert(false);
        __builtin_unreachable();
    }



    N *N::minimum(N* node) {
        // Find the leaf with smallest key
        if (!node)
            return NULL;

        if (node->getType() == NTypes::Nleaf){
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

        if (node->getType() == NTypes::Nleaf)
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




    N *N::getChild(const uint8_t k, N *node) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                return n->getChild(k);
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                return n->getChild(k);
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                return n->getChild(k);
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                return n->getChild(k);
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    void N::deleteChildren(N *node) {
        // if (N::isLeaf(node)) {
        //     return;
        // }
        if(node->getType() == NTypes::Nleaf){
            auto n = static_cast<LeafNode *>(node);
            delete n;
            return;
        }
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
    void N::removeAndShrink(curN *n, N *parentNode, uint8_t keyParent, uint8_t key) {
        if (n->remove(key, parentNode == nullptr)) {
            return;
        }

        auto nSmall = new smallerN(n->getPrefix(), n->getPrefixLength());


        n->remove(key, true);
        //n->copyTo(nSmall);
        N::change(parentNode, keyParent, nSmall);

        delete n;
    }

    void N::removeA(N *node, uint8_t key, N *parentNode, uint8_t keyParent) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                n->remove(key, false);
                return;
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                removeAndShrink<N16, N4>(n, parentNode, keyParent, key);
                return;
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                removeAndShrink<N48, N16>(n, parentNode, keyParent, key);
                return;
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                removeAndShrink<N256, N48>(n, parentNode, keyParent, key);
                return;
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    uint32_t N::getPrefixLength() const {
        return prefixCount;
    }

    bool N::hasPrefix() const {//节点的前缀的长度大于0，表示有前缀
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

    //判断是否是一个叶子节点
    bool N::isLeaf(const N *n) {
        return (reinterpret_cast<uint64_t>(n) & (static_cast<uint64_t>(1) << 63)) == (static_cast<uint64_t>(1) << 63);
    }
    
    //把value 转化为1个N节点
    N *N::setLeaf(TID tid) {
        return reinterpret_cast<N *>(tid | (static_cast<uint64_t>(1) << 63));
    }
    //将N节点类型 转化为 value
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


    TID N::getAnyChildTid(N *n) {
        N *nextNode = n;
        N *node = nullptr;

        nextNode = getAnyChild(nextNode);

        assert(nextNode != nullptr);
        if (isLeaf(nextNode)) {
            return getLeaf(nextNode);
        }

        while (true) {
            node = nextNode;

            nextNode = getAnyChild(node);

            assert(nextNode != nullptr);
            if (isLeaf(nextNode)) {
                return getLeaf(nextNode);
            }
        }
    }

    void N::getChildren(const N *node, uint8_t start, uint8_t end, std::tuple<uint8_t, N *> children[],
                        uint32_t &childrenCount) {
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<const N4 *>(node);
                n->getChildren(start, end, children, childrenCount);
                return;
            }
            case NTypes::N16: {
                auto n = static_cast<const N16 *>(node);
                n->getChildren(start, end, children, childrenCount);
                return;
            }
            case NTypes::N48: {
                auto n = static_cast<const N48 *>(node);
                n->getChildren(start, end, children, childrenCount);
                return;
            }
            case NTypes::N256: {
                auto n = static_cast<const N256 *>(node);
                n->getChildren(start, end, children, childrenCount);
                return;
            }
        }
    }
}
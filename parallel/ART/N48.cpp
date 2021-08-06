#include <assert.h>
#include <algorithm>
#include "N.h"



namespace ART_unsynchronized {

    bool N48::insert(uint8_t key, N *n) {
        if (count == 48) {
            return false;
        }
        unsigned pos = count;
        if (children[pos]) {
            for (pos = 0; children[pos] != nullptr; pos++);
        }
        children[pos] = n;
        childIndex[key] = (uint8_t) pos;
        count++;
        minl = std::min(minl,key);// add update bound
        maxr = std::max(maxr,key);
        return true;
    }
    
    
    // 有可能生成N16的节点 我tm也是服了
    template<class NODE> 
    void N48::copyTo(NODE *n) const {
        uint8_t lmin=255,rmax=0;
        for (unsigned i = 0; i < 256; i++) {
            if (childIndex[i] != emptyMarker) {
                n->insert(i, children[childIndex[i]]);// n是 node256类型
                lmin = std::min((uint8_t)i,lmin);
                rmax = std::max((uint8_t)i,rmax);
                if(children[childIndex[i]]->getType() != NTypes::Nleaf){
                    printf("n48 parent\n");
                    children[childIndex[i]]->parent = n;//当内部节点扩展的时候 node48的孩子 add parent

                }

                //  if(!isLeaf(this->children[childIndex[i]])){
                //      this->children[childIndex[i]]->parent = n;//当内部节点扩展的时候 node48的孩子 add parent
                // }

            }
        }
        n->parent = this->parent;  
        //init n256 bound
        n->minl = lmin;
        n->maxr = rmax;
        

        // auto node = static_cast<N256 *>(n);
        // node->minl = lmin;
        // node->maxr = rmax;
        
    }

    void N48::change(uint8_t key, N *val) {
        children[childIndex[key]] = val;
    }

    N *N48::getChild(const uint8_t k) const {
        if (childIndex[k] == emptyMarker) {
            return nullptr;
        } else {
            return children[childIndex[k]];
        }
    }

    bool N48::remove(uint8_t k, bool force) {
        if (count == 12 && !force) {
            return false;
        }
        assert(childIndex[k] != emptyMarker);
        children[childIndex[k]] = nullptr;
        childIndex[k] = emptyMarker;
        count--;
        assert(getChild(k) == nullptr);
        return true;
    }

    N *N48::getAnyChild() const {
        N *anyChild = nullptr;
        for (unsigned i = 0; i < 256; i++) {
            if (childIndex[i] != emptyMarker) {
                if (N::isLeaf(children[childIndex[i]])) {
                    return children[childIndex[i]];
                } else {
                    anyChild = children[childIndex[i]];
                };
            }
        }
        return anyChild;
    }

    void N48::deleteChildren() {
        for (unsigned i = 0; i < 256; i++) {
            if (childIndex[i] != emptyMarker) {
                N::deleteChildren(children[childIndex[i]]);
                N::deleteNode(children[childIndex[i]]);
            }
        }
    }

    void N48::getChildren(uint8_t start, uint8_t end, std::tuple<uint8_t, N *> *&children,
                          uint32_t &childrenCount) const {
        childrenCount = 0;
        for (unsigned i = start; i <= end; i++) {
            if (this->childIndex[i] != emptyMarker) {
                children[childrenCount] = std::make_tuple(i, this->children[this->childIndex[i]]);
                childrenCount++;
            }
        }
    }
}
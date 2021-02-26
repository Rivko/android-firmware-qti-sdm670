#ifndef AEEQLISTEXT_H
#define AEEQLISTEXT_H

#include "AEEQList.h"
#include "AEEstd.h"

#ifndef QLIST_ATTRIBUTE_UNUSED
#ifdef _WIN32
#define QLIST_ATTRIBUTE_UNUSED
#else
#define QLIST_ATTRIBUTE_UNUSED __attribute__ ((unused))
#endif
#endif // QLIST_ATTRIBUTE_UNUSED


#ifdef _WIN32
#define QLIST_FQN(pIter, node, mem) (void*)(((char*)(node)) - (((char*)(&(pIter)->mem)) - (char*)(pIter)))
#else
#define QLIST_FQN(pIter, node, mem) (void*)(((char*)(node)) - (((char*)(&(((typeof(pIter))0)->mem))) - (char*)((typeof(pIter))0)))
#endif

#define QLIST_TQN(pIter, mem) (&((pIter)->mem))
#define QLIST_TNQN(pIter, mem) ((pIter) == 0 ? 0 : QLIST_TQN(pIter, mem)->pNext)

#define QLIST_NEXTSAFE_FOR_ALLX(pList, pIter, pNodeNext, member)            \
     for ((pIter) = QLIST_FQN(pIter, (pList)->n.pNext, member), (pNodeNext) = QLIST_TNQN(pIter, member); \
          (pIter) != QLIST_FQN(pIter, &(pList)->n, member); \
          (pIter) = QLIST_FQN(pIter, (pNodeNext), member), (pNodeNext) = QLIST_TNQN(pIter, member))

/** @param name, name of the typed list, so you can do name lst
  * @param type, the type of list
  * @param qn, QNode member
  * 
  *   DEFINE_TQLIST(mylist,struct foo, qn)
  *   mylist myl;
  *   struct foo* pnn, *iter;
  *   mylist_ctor(&myl);
  *
  *   for(iter = mylist_start(&myl, &pnn);
  *       iter != 0;
  *       iter = mylist_safe_next(&myl,&pnn)) {
  *
  *       mylist_dequeuez(iter);
  *   }
  */
#define DEFINE_TQLIST(name,type,qn) \
   typedef struct name {\
      QList ql;\
   } name;\
   QLIST_ATTRIBUTE_UNUSED \
   static __inline void name##_ctor(name * l) {\
      QList_Ctor(&l->ql);\
   }\
   QLIST_ATTRIBUTE_UNUSED \
   static __inline void name##_append(name * l, type * n) {\
      QNode_DequeueZ(&n->qn);\
      QList_AppendNode(&l->ql, &n->qn);\
   }\
   QLIST_ATTRIBUTE_UNUSED \
   static __inline void name##_prepend(name * l, type * n) {\
      QNode_DequeueZ(&n->qn);\
      QList_PrependNode(&l->ql, &n->qn);\
   }\
   QLIST_ATTRIBUTE_UNUSED \
   static __inline type * name##_popz(name * l) {\
      QNode* pn = QList_PopZ(&l->ql); \
      return pn == 0 ? 0 : STD_RECOVER_REC(type, qn, pn); \
   }\
   QLIST_ATTRIBUTE_UNUSED \
   static __inline boolean name##_isempty(name * l) {\
      return QList_IsEmpty(&l->ql);\
   }\
   QLIST_ATTRIBUTE_UNUSED \
   static __inline void name##_dequeuez(type * n) {\
      QNode_DequeueZ(&n->qn);\
   }\
   QLIST_ATTRIBUTE_UNUSED \
   static __inline type *  name##_start(name * l, type ** nn) {\
      QNode* start = QList_GetFirst(&l->ql);  \
      QNode* next = start == 0 ? 0 : start->pNext; \
      QNode* current = start;\
      type * rv = current == 0 ? 0 : STD_RECOVER_REC(type, qn, current); \
      *nn = next == 0 ? 0 : STD_RECOVER_REC(type, qn, next); \
      return rv; \
   }\
   QLIST_ATTRIBUTE_UNUSED \
   static __inline type *  name##_safe_next(name * l, type ** nn) {\
      QNode* current = &(*nn)->qn; \
      QNode* next = current->pNext; \
      type * rv = current == &l->ql.n ? 0 : STD_RECOVER_REC(type, qn, current); \
      *nn = STD_RECOVER_REC(type, qn, next); \
      return rv; \
   }


#endif // AEEQLISTEXT_H

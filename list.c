//Lists
//     A list is headed by a structure defined by the LIST_HEAD macro.  This
//     structure contains a single pointer to the first element on the list.
//     The elements are doubly linked so that an arbitrary element can be
//     removed without traversing the list.  New elements can be added to the
//     list after an existing element, before an existing element, or at the
//     head of the list.  A LIST_HEAD structure is declared as follows:
//
//           LIST_HEAD(HEADNAME, TYPE) head;
//
//     where HEADNAME is the name of the structure to be defined, and TYPE is
//     the type of the elements to be linked into the list.  A pointer to the
//     head of the list can later be declared as:
//
//           struct HEADNAME *headp;
//
//     (The names head and headp are user selectable.)
//
//     The macro LIST_HEAD_INITIALIZER evaluates to an initializer for the
//     list head.
//
//     The macro LIST_EMPTY evaluates to true if there are no elements in the
//     list.
//
//     The macro LIST_ENTRY declares a structure that connects the elements in
//     the list.
//
//     The macro LIST_FIRST returns the first element in the list or NULL if
//     the list is empty.
//
//     The macro LIST_FOREACH traverses the list referenced by head in the
//     forward direction, assigning each element in turn to var.
//
//     The macro LIST_INIT initializes the list referenced by head.
//
//     The macro LIST_INSERT_HEAD inserts the new element elm at the head of
//     the list.
//
//     The macro LIST_INSERT_AFTER inserts the new element elm after the ele‐
//     ment listelm.
//
//     The macro LIST_INSERT_BEFORE inserts the new element elm before the
//     element listelm.
//
//     The macro LIST_NEXT returns the next element in the list, or NULL if
//     this is the last.
//
//     The macro LIST_REMOVE removes the element elm from the list.
//
//   List example
//
//     LIST_HEAD(listhead, entry) head =
//         LIST_HEAD_INITIALIZER(head);
//     struct listhead *headp;                 /* List head. */
//     struct entry {
//             ...
//             LIST_ENTRY(entry) entries;      /* List. */
//             ...
//     } *n1, *n2, *n3, *np, *np_temp;
//
//     LIST_INIT(&head);                       /* Initialize the list. */
//
//     n1 = malloc(sizeof(struct entry));      /* Insert at the head. */
//     LIST_INSERT_HEAD(&head, n1, entries);
//
//     n2 = malloc(sizeof(struct entry));      /* Insert after. */
//     LIST_INSERT_AFTER(n1, n2, entries);
//
//     n3 = malloc(sizeof(struct entry));      /* Insert before. */
//     LIST_INSERT_BEFORE(n2, n3, entries);
//
//     LIST_REMOVE(n2, entries);               /* Deletion. */
//     free(n2);
//                                             /* Forward traversal. */
//     LIST_FOREACH(np, &head, entries)
//             np-> ...
//
//     while (!LIST_EMPTY(&head)) {            /* List Deletion. */
//             n1 = LIST_FIRST(&head);
//             LIST_REMOVE(n1, entries);
//             free(n1);
//     }
//
//     n1 = LIST_FIRST(&head);                 /* Faster List Deletion. */
//     while (n1 != NULL) {
//             n2 = LIST_NEXT(n1, entries);
//             free(n1);
//             n1 = n2;
//     }
//     LIST_INIT(&head);

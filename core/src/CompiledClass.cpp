#include "CompiledClass.h"
#include "ClassMember.h"
#include "ConceptInterpreter.h"

POOLED_IMPLEMENTATION(CompiledClass)

struct CompiledClass *new_CompiledClass(void *PIF, const ClassCode *CC) {
    CompiledClass *self = (struct CompiledClass *)AllocClassObject(PIF);
    self->_Class   = CC;
    self->_CONTEXT = 0;
    self->LINKS    = 1;
    //---------------------------------
    INTEGER Count = self->_Class->DataMembersCount;
    if (Count) {
        self->_CONTEXT = (VariableDATA **)FAST_MALLOC(PIF, sizeof(VariableDATA *) * Count);
        for (INTEGER i = 0; i < Count; i++)
            self->_CONTEXT[i] = 0;
    }
    return self;
}

VariableDATA *CompiledClass_CreateVariable(struct CompiledClass *self, INTEGER reloc, ClassMember *CM, VariableDATA *CONTAINER) {
    VariableDATA *_CONTEXT_i = self->_CONTEXT[reloc];

    if (!_CONTEXT_i) {
        if ((CM) && (!CM->IS_FUNCTION)) {
            PIFAlizator *PIF = GET_PIF(self);
            if (CONTAINER) {
                _CONTEXT_i = CONTAINER;
                CLASS_CHECK(CONTAINER, NULL);
                CONTAINER->LINKS++;
            } else {
                _CONTEXT_i        = (VariableDATA *)VAR_ALLOC(PIF);
                // ensure no "garbage" gets to the garbage collector in AllocArray
                _CONTEXT_i->TYPE  = VARIABLE_NUMBER;
                _CONTEXT_i->LINKS = 1;
            }
            self->_CONTEXT[reloc] = _CONTEXT_i;
            if (CM->VD) {
                if (CM->VD->TYPE == VARIABLE_ARRAY) {
                    _CONTEXT_i->CLASS_DATA = new_Array(PIF);
                } else
                if (CM->VD->TYPE == VARIABLE_STRING) {
                    NEW_CONCEPT_STRING_BUFFER(_CONTEXT_i, CM->VD->value.c_str(), CM->VD->value.Length());
                } else
                    _CONTEXT_i->NUMBER_DATA = CM->VD->nValue;

                _CONTEXT_i->TYPE = CM->VD->TYPE;

                _CONTEXT_i->IS_PROPERTY_RESULT = 0;
            } else {
                _CONTEXT_i->NUMBER_DATA        = 0;
                _CONTEXT_i->TYPE               = VARIABLE_NUMBER;
                _CONTEXT_i->IS_PROPERTY_RESULT = 0;
            }
        }
    }
    return _CONTEXT_i;
}

const char *CompiledClass_GetClassName(const struct CompiledClass *self) {
    if (self->_Class) {
        return self->_Class->NAME.c_str();
    }
    return NULL_STRING;
}

void CompiledClass__GO_GARBAGE(struct CompiledClass *self, void *PIF, GarbageCollector *__gc_obj, GarbageCollector *__gc_array, GarbageCollector *__gc_vars, signed char check_objects) {
    if (self->LINKS < 0)
        return;

    // ensure is not deleted
    self->LINKS++;

#ifdef USE_RECURSIVE_MARKINGS
    if (self->_Class->DESTRUCTOR)
        CompiledClass_Destroy(self, (PIFAlizator *)PIF);

    if (!self->_CONTEXT) {
        self->LINKS = -1;
        return;
    }

    VariableDATA **r_CONTEXT = self->_CONTEXT;
    self->_CONTEXT = NULL;

    INTEGER Count = _Class->DataMembersCount;

    for (INTEGER i = 0; i < Count; i++) {
        if (r_CONTEXT [i]) {
            // ensure is not deleted
            r_CONTEXT [i]->LINKS++;
            __gc_vars->Reference(r_CONTEXT[i]);
            void *orig_data = r_CONTEXT [i]->CLASS_DATA;
            if (orig_data) {
                if ((r_CONTEXT [i]->TYPE == VARIABLE_CLASS) || (r_CONTEXT [i]->TYPE == VARIABLE_DELEGATE)) {
                    if (orig_data != self) {
                        __gc_obj->Reference(orig_data);
                        void *CLASS_DATA = Var->CLASS_DATA;
                        if (Var->TYPE == VARIABLE_DELEGATE) {
                            CLASS_DATA = delegate_Class(CLASS_DATA);
                            free_Delegate(Var->CLASS_DATA);
                            Var->CLASS_DATA = NULL;
                        }
                        ((struct CompiledClass *)CLASS_DATA)->__GO_GARBAGE(PIF, __gc_obj, __gc_array, __gc_vars);
                    }
                    orig_data = 0;
                } else
                if (r_CONTEXT [i]->TYPE == VARIABLE_ARRAY) {
                    __gc_array->Reference(orig_data);
                    ((struct Array *)orig_data)->__GO_GARBAGE(PIF, __gc_obj, __gc_array, __gc_vars);
                    orig_data = 0;
                }
            }
        }
    }
    FAST_FREE(r_CONTEXT);
    self->LINKS = -1;
#else
    int           inspectSize = INITIAL_INSPECT_SIZE;
    struct CompiledClass **toInspect = (struct CompiledClass **)realloc(NULL, sizeof(struct CompiledClass *) * inspectSize);
    toInspect[0] = self;
    // self->LINKS = 1;
    int inspectPos = 1;

    for (int j = 0; j < inspectPos; j++) {
        struct CompiledClass *CC = toInspect[j];
        if (CC->LINKS < 0)
            continue;

        CC->LINKS++;

        // more efficient than calling CallAllDestructors, but it crashes with some cyclic object references
        // if (CC->_Class->DESTRUCTOR)
        //    CompiledClass_Destroy(CC, (PIFAlizator *)PIF);

        // CC->LINKS = -1;

        if (CC->_CONTEXT) {
            const ClassCode *base = CC->_Class;
            for (int i = 0; i < base->DataMembersCount; i++) {
                VariableDATA *Var = CC->_CONTEXT[i];
                if (Var) {
                    if (check_objects == -1) {
                        Var->LINKS++;
                        __gc_vars->Reference(Var);
                    } else {
                        if (Var->LINKS <= 1)
                            __gc_vars->Reference(Var);
                        else
                            Var->LINKS--;
                    }
                    if (Var->CLASS_DATA) {
                        if ((Var->TYPE == VARIABLE_CLASS) || (Var->TYPE == VARIABLE_DELEGATE)) {
                            void *CLASS_DATA = Var->CLASS_DATA;
                            if (Var->TYPE == VARIABLE_DELEGATE) {
                                CLASS_DATA = delegate_Class(CLASS_DATA);
                                free_Delegate(Var->CLASS_DATA);
                                Var->CLASS_DATA = NULL;
                            }
                            CompiledClass *CC2 = (struct CompiledClass *)CLASS_DATA;
                            if (CC2->LINKS >= 0) {
                                if ((check_objects == -1) || ((CC2->reachable & check_objects) != check_objects)) {
                                    if (check_objects != -1)
                                        CC2->reachable = check_objects;
                                    if (!__gc_obj->IsReferenced(CC2)) {
                                        __gc_obj->Reference(CC2);
                                        if (CC2->_CONTEXT) {
                                            if (inspectPos < inspectSize)
                                                toInspect[inspectPos++] = CC2;
                                            else {
                                                inspectSize            += INSPECT_INCREMENT;
                                                toInspect               = (struct CompiledClass **)realloc(toInspect, sizeof(struct CompiledClass *) * inspectSize);
                                                toInspect[inspectPos++] = CC2;
                                            }
                                        }
                                    }
                                } else {
                                    RESET_VARIABLE(Var, NULL);
                                }
                            }
                            Var->CLASS_DATA = 0;
                            Var->TYPE = VARIABLE_NUMBER;
                        } else
                        if (Var->TYPE == VARIABLE_ARRAY) {
                            if ((check_objects == -1) || ((((struct Array *)Var->CLASS_DATA)->reachable & check_objects) != check_objects)) {
                                if (check_objects != -1)
                                    ((struct Array *)Var->CLASS_DATA)->reachable = check_objects;
                                if (!__gc_array->IsReferenced(Var->CLASS_DATA)) {
                                    __gc_array->Reference(Var->CLASS_DATA);
                                    Array_GO_GARBAGE((struct Array *)Var->CLASS_DATA, PIF, __gc_obj, __gc_array, __gc_vars, check_objects);
                                    // ensure never visited again
                                    Var->CLASS_DATA = 0;
                                    Var->TYPE = VARIABLE_NUMBER; 
                                }
                            } else {
                                RESET_VARIABLE(Var, NULL);
                            }
                        }
                    }
                }
            }
        }
    }
    free(toInspect);
#endif
}

const ClassCode *CompiledClass_GetClass(const struct CompiledClass *self) {
    return self->_Class;
}

VariableDATA **CompiledClass_GetContext(const struct CompiledClass *self) {
    return self->_CONTEXT;
}

int CompiledClass_Destroy(struct CompiledClass *self, PIFAlizator *PIF, SCStack *STACK_TRACE) {
    self->LINKS++;
    VariableDATA *THROW_DATA = 0;

    STACK(STACK_TRACE, self->_Class->DESTRUCTOR_MEMBER->_DEBUG_STARTLINE)
    VariableDATA * RESULT = self->_Class->DESTRUCTOR_MEMBER->Execute(PIF, self->_Class->CLSID, self, 0, self->_CONTEXT, THROW_DATA, STACK_TRACE, NULL, 0);
    UNSTACK;
    if (RESULT) {
        FREE_VARIABLE(RESULT, STACK_TRACE);
    }

    // avoid double deleting of the class !!!
    if (THROW_DATA) {
        FREE_VARIABLE(THROW_DATA, STACK_TRACE);
        AnsiException *Exc = new AnsiException(ERR635, 0, 635, "", self->_Class->_DEBUG_INFO_FILENAME, self->_Class->NAME, self->_Class->DESTRUCTOR_MEMBER->NAME);
        PIF->AcknoledgeRunTimeError(STACK_TRACE, Exc);
    }
    self->LINKS = -1;
    return 1;
}

int CompiledClass_HasDestructor(const struct CompiledClass *self) {
    if (self->LINKS < 0) {
        return 0;
    }

    if (self->_Class->DESTRUCTOR) {
        return 1;
    }

    return 0;
}

void CompiledClass_UnlinkObjects(struct CompiledClass *self) {
    if (self->LINKS < 0) {
        return;
    }
    self->LINKS = -1;

    INTEGER Count = self->_Class->DataMembersCount;

    for (INTEGER i = 0; i < Count; i++) {
        VariableDATA *_CONTEXT_i = self->_CONTEXT [i];
        if (_CONTEXT_i) {
            // delete no object ! ... it's the garbage collector's job
            if (_CONTEXT_i->CLASS_DATA) {
                FREE_VARIABLE_NO_OBJECTS(_CONTEXT_i);
            }
        }
    }

    FAST_FREE(GET_PIF(self), self->_CONTEXT);
    self->_CONTEXT = 0;
}

void delete_CompiledClass(struct CompiledClass *self, SCStack *STACK_TRACE) {
    if (self->LINKS < 0) {
        FreeClassObject(self);
        return;
    }

    PIFAlizator *PIF          = GET_PIF(self);
    if (self->_Class->DESTRUCTOR)
        CompiledClass_Destroy(self, PIF, STACK_TRACE);

    self->LINKS = -1;
    INTEGER       Count       = self->_Class->DataMembersCount;
    CompiledClass **toInspect = NULL;
    unsigned int  inspectSize = 0;
    unsigned int  inspectPos  = 0;
    for (INTEGER i = 0; i < Count; i++) {
        VariableDATA *_CONTEXT_i = self->_CONTEXT [i];
        if (_CONTEXT_i) {
            //=======================================//
            if ((_CONTEXT_i->CLASS_DATA == self) && (_CONTEXT_i->TYPE == VARIABLE_CLASS)) {
                _CONTEXT_i->TYPE       = VARIABLE_NUMBER;
                _CONTEXT_i->CLASS_DATA = 0;
            }
            if ((_CONTEXT_i->TYPE == VARIABLE_DELEGATE) && (delegate_Class(_CONTEXT_i->CLASS_DATA) == self)) {
                free_Delegate(_CONTEXT_i->CLASS_DATA);
                _CONTEXT_i->TYPE       = VARIABLE_NUMBER;
                _CONTEXT_i->CLASS_DATA = 0;
            }
            //=======================================//
#ifdef USE_RECURSIVE_MARKINGS
            FREE_VARIABLE(_CONTEXT_i, STACK_TRACE);
#else
            _CONTEXT_i->LINKS--;
            if (!_CONTEXT_i->LINKS) {
                if (_CONTEXT_i->CLASS_DATA) {
                    if (_CONTEXT_i->TYPE == VARIABLE_STRING) {
                        plainstring_delete((struct plainstring *)_CONTEXT_i->CLASS_DATA);
                    } else
                    if ((_CONTEXT_i->TYPE == VARIABLE_CLASS) || (_CONTEXT_i->TYPE == VARIABLE_DELEGATE)) {
                        void *CLASS_DATA = _CONTEXT_i->CLASS_DATA;
                        if (_CONTEXT_i->TYPE == VARIABLE_DELEGATE) {
                            CLASS_DATA = delegate_Class(CLASS_DATA);
                            free_Delegate(_CONTEXT_i->CLASS_DATA);
                            _CONTEXT_i->CLASS_DATA = NULL;
                        }
                        if (!--((struct CompiledClass *)CLASS_DATA)->LINKS) {
                            if (inspectPos >= inspectSize) {
                                inspectSize += INSPECT_INCREMENT;
                                toInspect    = (CompiledClass **)FAST_REALLOC(PIF, toInspect, sizeof(struct CompiledClass *) * inspectSize);
                            }
                            toInspect[inspectPos++] = (struct CompiledClass *)CLASS_DATA;
                        }
                    } else
                    if (_CONTEXT_i->TYPE == VARIABLE_ARRAY) {
                        if (!--((struct Array *)_CONTEXT_i->CLASS_DATA)->LINKS)
                            delete_Array((struct Array *)_CONTEXT_i->CLASS_DATA);
                    }
                }
                VAR_FREE(_CONTEXT_i);
            }
#endif
        }
    }
    FAST_FREE(PIF, self->_CONTEXT);
#ifndef USE_RECURSIVE_MARKINGS
    if (inspectPos) {
        for (unsigned int j = 0; j < inspectPos; j++) {
            CompiledClass *obj = toInspect[j];
            if (CompiledClass_HasDestructor(obj)) {
                obj->LINKS = 2;
                CompiledClass_Destroy(obj, PIF);
            }
            obj->LINKS = -1;
            if (obj->_CONTEXT) {
                INTEGER Count = obj->_Class->DataMembersCount;
                for (INTEGER i = 0; i < Count; i++) {
                    VariableDATA *_CONTEXT_i = obj->_CONTEXT [i];
                    if (_CONTEXT_i) {
                        //=======================================//
                        if ((_CONTEXT_i->CLASS_DATA == obj) && (_CONTEXT_i->TYPE == VARIABLE_CLASS)) {
                            _CONTEXT_i->TYPE       = VARIABLE_NUMBER;
                            _CONTEXT_i->CLASS_DATA = 0;
                        }
                        if ((_CONTEXT_i->TYPE == VARIABLE_DELEGATE) && (delegate_Class(_CONTEXT_i->CLASS_DATA) == obj)) {
                            free_Delegate(_CONTEXT_i->CLASS_DATA);
                            _CONTEXT_i->TYPE       = VARIABLE_NUMBER;
                            _CONTEXT_i->CLASS_DATA = 0;
                        }
                        //=======================================//
                        _CONTEXT_i->LINKS--;
                        if (!_CONTEXT_i->LINKS) {
                            if (_CONTEXT_i->CLASS_DATA) {
                                if (_CONTEXT_i->TYPE == VARIABLE_STRING) {
                                    plainstring_delete((struct plainstring *)_CONTEXT_i->CLASS_DATA);
                                } else
                                if ((_CONTEXT_i->TYPE == VARIABLE_CLASS) || (_CONTEXT_i->TYPE == VARIABLE_DELEGATE)) {
                                    void *CLASS_DATA = _CONTEXT_i->CLASS_DATA;
                                    if (_CONTEXT_i->TYPE == VARIABLE_DELEGATE) {
                                        CLASS_DATA = delegate_Class(CLASS_DATA);
                                        free_Delegate(_CONTEXT_i->CLASS_DATA);
                                        _CONTEXT_i->CLASS_DATA = NULL;
                                    }
                                    if (!--((struct CompiledClass *)CLASS_DATA)->LINKS) {
                                        if (inspectPos >= inspectSize) {
                                            if (inspectSize < 0xFFFF)
                                                inspectSize += INSPECT_INCREMENT;
                                            else
                                                inspectSize += inspectSize/2;
                                            toInspect = (CompiledClass **)FAST_REALLOC(PIF, toInspect, sizeof(struct CompiledClass *) * inspectSize);
                                        }
                                        toInspect[inspectPos++] = (struct CompiledClass *)CLASS_DATA;
                                    }
                                } else
                                if (_CONTEXT_i->TYPE == VARIABLE_ARRAY) {
                                    if (!--((struct Array *)_CONTEXT_i->CLASS_DATA)->LINKS)
                                        delete_Array((struct Array *)_CONTEXT_i->CLASS_DATA);
                                }
                            }
                            VAR_FREE(_CONTEXT_i);
                        }
                    }
                }
                FAST_FREE(PIF, obj->_CONTEXT);
                obj->_CONTEXT = NULL;
            }
            FreeClassObject(obj);
        }
        FAST_FREE(PIF, toInspect);
    }
#endif
    FreeClassObject(self);
}

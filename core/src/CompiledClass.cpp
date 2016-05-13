#include "CompiledClass.h"
#include "ClassMember.h"
#include "ConceptInterpreter.h"

POOLED_IMPLEMENTATION(CompiledClass)

CompiledClass::CompiledClass(ClassCode *CC) {
    _Class   = CC;
    _CONTEXT = 0;
    LINKS    = 1;
    //---------------------------------
    INTEGER Count = _Class->DataMembersCount;
    if (Count) {
        _CONTEXT = (VariableDATA **)FAST_MALLOC(sizeof(VariableDATA *) * Count);
        for (register INTEGER i = 0; i < Count; i++)
            _CONTEXT[i] = 0;
    }
}

VariableDATA *CompiledClass::CreateVariable(INTEGER reloc, ClassMember *CM, VariableDATA *CONTAINER) {
    VariableDATA *_CONTEXT_i = _CONTEXT[reloc];

    if (!_CONTEXT_i) {
        if ((CM) && (!CM->IS_FUNCTION)) {
            PIFAlizator *PIF = GET_PIF(this);
            if (CONTAINER) {
                _CONTEXT_i = CONTAINER;
                CLASS_CHECK(CONTAINER);
                CONTAINER->LINKS++;
            } else {
                _CONTEXT_i        = (VariableDATA *)VAR_ALLOC(PIF);
                _CONTEXT_i->LINKS = 1;
            }
            _CONTEXT[reloc] = _CONTEXT_i;
            if (CM->VD) {
                if (CM->VD->TYPE == VARIABLE_ARRAY) {
                    _CONTEXT_i->CLASS_DATA = new(AllocArray( PIF))Array( PIF);
                } else
                if (CM->VD->TYPE == VARIABLE_STRING) {
                    _CONTEXT_i->CLASS_DATA     = NULL;
                    CONCEPT_STRING(_CONTEXT_i) = CM->VD->value;
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

AnsiString CompiledClass::GetClassName() {
    if (_Class) {
        return _Class->NAME;
    }
    return NULL_STRING;
}

void CompiledClass::__GO_GARBAGE(void *PIF, GarbageCollector *__gc_obj, GarbageCollector *__gc_array, GarbageCollector *__gc_vars, signed char check_objects) {
    if (this->LINKS < 0)
        return;

    // ensure is not deleted
    this->LINKS++;
    if (_Class->DESTRUCTOR)
        Destroy((PIFAlizator *)PIF);

    if (!this->_CONTEXT)
        return;
#ifdef USE_RECURSIVE_MARKINGS
    VariableDATA **r_CONTEXT = this->_CONTEXT;
    this->_CONTEXT = NULL;

    INTEGER Count = _Class->DataMembersCount;

    for (INTEGER i = 0; i < Count; i++) {
        if (r_CONTEXT [i]) {
            // ensure is not deleted
            r_CONTEXT [i]->LINKS++;
            __gc_vars->Reference(r_CONTEXT[i]);
            void *orig_data = r_CONTEXT [i]->CLASS_DATA;
            if (orig_data) {
                if ((r_CONTEXT [i]->TYPE == VARIABLE_CLASS) || (r_CONTEXT [i]->TYPE == VARIABLE_DELEGATE)) {
                    if (orig_data != this) {
                        __gc_obj->Reference(orig_data);
                        ((CompiledClass *)orig_data)->__GO_GARBAGE(PIF, __gc_obj, __gc_array, __gc_vars);
                    }
                    orig_data = 0;
                } else
                if (r_CONTEXT [i]->TYPE == VARIABLE_ARRAY) {
                    __gc_array->Reference(orig_data);
                    ((Array *)orig_data)->__GO_GARBAGE(PIF, __gc_obj, __gc_array, __gc_vars);
                    orig_data = 0;
                }
            }
        }
    }
    FAST_FREE(r_CONTEXT);
    this->LINKS = -1;
#else
    int           inspectSize = INITIAL_INSPECT_SIZE;
    CompiledClass **toInspect = (CompiledClass **)realloc(NULL, sizeof(CompiledClass *) * inspectSize);
    toInspect[0] = this;
    int inspectPos = 1;

    for (int j = 0; j < inspectPos; j++) {
        CompiledClass *CC = toInspect[j];
        if (CC->LINKS < 0)
            continue;

        CC->LINKS++;
        if (CC->_Class->DESTRUCTOR)
            CC->Destroy((PIFAlizator *)PIF);

        CC->LINKS = -1;

        if (CC->_CONTEXT) {
            ClassCode *base = CC->_Class;
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
                            CompiledClass *CC2 = (CompiledClass *)Var->CLASS_DATA;
                            if (CC2->LINKS >= 0) {
                                if ((check_objects == -1) || ((CC2->reachable & check_objects) != check_objects)) {
                                    if (check_objects != -1)
                                        CC2->reachable = check_objects;
                                    __gc_obj->Reference(CC2);
                                    if (CC2->_CONTEXT) {
                                        if (inspectPos < inspectSize)
                                            toInspect[inspectPos++] = CC2;
                                        else {
                                            inspectSize            += INSPECT_INCREMENT;
                                            toInspect               = (CompiledClass **)realloc(toInspect, sizeof(CompiledClass *) * inspectSize);
                                            toInspect[inspectPos++] = CC2;
                                        }
                                    }
                                } else {
                                    RESET_VARIABLE(Var);
                                }
                            }
                            Var->CLASS_DATA = 0;
                        } else
                        if (Var->TYPE == VARIABLE_ARRAY) {
                            if ((check_objects == -1) || ((((Array *)Var->CLASS_DATA)->reachable & check_objects) != check_objects)) {
                                if (check_objects != -1)
                                    ((Array *)Var->CLASS_DATA)->reachable = check_objects;
                                __gc_array->Reference(Var->CLASS_DATA);
                                ((Array *)Var->CLASS_DATA)->__GO_GARBAGE(PIF, __gc_obj, __gc_array, __gc_vars, check_objects);
                                Var->CLASS_DATA = 0;
                            } else {
                                RESET_VARIABLE(Var);
                            }
                        }
                    }
                }
            }
            FAST_FREE(CC->_CONTEXT);
            CC->_CONTEXT = NULL;
        }
    }
    free(toInspect);
#endif
}

ClassCode *CompiledClass::GetClass() {
    return _Class;
}

VariableDATA **CompiledClass::GetContext() {
    return _CONTEXT;
}

int CompiledClass::Destroy(PIFAlizator *PIF) {
    VariableDATA *OWNER = (VariableDATA *)VAR_ALLOC(PIF);

    OWNER->TYPE       = VARIABLE_CLASS;
    OWNER->LINKS      = 1;
    OWNER->CLASS_DATA = this;
    OWNER->IS_PROPERTY_RESULT = 0;
    this->LINKS++;
    VariableDATA *THROW_DATA = 0;

    STACK(NULL, _Class->DESTRUCTOR_MEMBER->_DEBUG_STARTLINE)
    VariableDATA * RESULT = _Class->DESTRUCTOR_MEMBER->Execute(PIF, this->_Class->CLSID, OWNER, 0, _CONTEXT, THROW_DATA, NULL);
    UNSTACK;
    if (RESULT) {
        FREE_VARIABLE(RESULT);
    }

    // avoid double deleting of the class !!!
    OWNER->TYPE       = VARIABLE_NUMBER;
    OWNER->CLASS_DATA = 0;
    FREE_VARIABLE(OWNER);

    if (THROW_DATA) {
        FREE_VARIABLE(THROW_DATA);
        AnsiException *Exc = new AnsiException(ERR635, 0, 635, 0, _Class->_DEBUG_INFO_FILENAME, _Class->NAME, _Class->DESTRUCTOR_MEMBER->NAME);
         PIF->AcknoledgeRunTimeError(NULL, Exc);
    }
    this->LINKS = -1;
    return 1;
}

int CompiledClass::HasDestructor() {
    if (this->LINKS < 0) {
        return 0;
    }

    if (_Class->DESTRUCTOR) {
        return 1;
    }

    return 0;
}

void CompiledClass::UnlinkObjects() {
    if (this->LINKS < 0) {
        return;
    }
    this->LINKS = -1;

    INTEGER Count = _Class->DataMembersCount;

    for (register INTEGER i = 0; i < Count; i++) {
        VariableDATA *_CONTEXT_i = _CONTEXT [i];
        if (_CONTEXT_i) {
            // delete no object ! ... it's the garbage collector's job
            if (_CONTEXT_i->CLASS_DATA) {

                FREE_VARIABLE_NO_OBJECTS(_CONTEXT_i);
            }
        }
    }

    FAST_FREE(_CONTEXT);
    _CONTEXT = 0;
}

CompiledClass::~CompiledClass() {
    if (this->LINKS < 0)
        return;

    if (_Class->DESTRUCTOR) {
        PIFAlizator *PIF          = GET_PIF(this);
        Destroy(PIF);
    }

    this->LINKS = -1;
    INTEGER       Count       = _Class->DataMembersCount;
    CompiledClass **toInspect = NULL;
    unsigned int  inspectSize = 0;
    unsigned int  inspectPos  = 0;
    for (register INTEGER i = 0; i < Count; i++) {
        VariableDATA *_CONTEXT_i = _CONTEXT [i];
        if (_CONTEXT_i) {
            //=======================================//
            if ((_CONTEXT_i->CLASS_DATA == this) && ((_CONTEXT_i->TYPE == VARIABLE_CLASS) || (_CONTEXT_i->TYPE == VARIABLE_DELEGATE))) {
                _CONTEXT_i->TYPE       = VARIABLE_NUMBER;
                _CONTEXT_i->CLASS_DATA = 0;
            }
            //=======================================//
#ifdef USE_RECURSIVE_MARKINGS
            FREE_VARIABLE(_CONTEXT_i);
#else
            _CONTEXT_i->LINKS--;
            if (_CONTEXT_i->LINKS < 1) {
                if (_CONTEXT_i->CLASS_DATA) {
                    if (_CONTEXT_i->TYPE == VARIABLE_STRING) {
                        delete (AnsiString *)_CONTEXT_i->CLASS_DATA;
                    } else
                    if ((_CONTEXT_i->TYPE == VARIABLE_CLASS) || (_CONTEXT_i->TYPE == VARIABLE_DELEGATE)) {
                        if (!--((CompiledClass *)_CONTEXT_i->CLASS_DATA)->LINKS) {
                            if (inspectPos >= inspectSize) {
                                inspectSize += INSPECT_INCREMENT;
                                toInspect    = (CompiledClass **)FAST_REALLOC(toInspect, sizeof(CompiledClass *) * inspectSize);
                            }
                            toInspect[inspectPos++] = (CompiledClass *)_CONTEXT_i->CLASS_DATA;
                        }
                    } else
                    if (_CONTEXT_i->TYPE == VARIABLE_ARRAY) {
                        if (!--((Array *)_CONTEXT_i->CLASS_DATA)->LINKS)
                            delete (Array *)_CONTEXT_i->CLASS_DATA;
                    }
                }
                VAR_FREE(_CONTEXT_i);
            }
#endif
        }
    }
    FAST_FREE(_CONTEXT);
#ifndef USE_RECURSIVE_MARKINGS
    if (inspectPos) {
        for (int j = 0; j < inspectPos; j++) {
            CompiledClass *obj = toInspect[j];
            if (obj->HasDestructor()) {
                obj->LINKS = 2;
                obj->Destroy(GET_PIF(this));
            }
            obj->LINKS = -1;
            if (obj->_CONTEXT) {
                INTEGER Count = obj->_Class->DataMembersCount;
                for (register INTEGER i = 0; i < Count; i++) {
                    VariableDATA *_CONTEXT_i = obj->_CONTEXT [i];
                    if (_CONTEXT_i) {
                        //=======================================//
                        if ((_CONTEXT_i->CLASS_DATA == obj) && ((_CONTEXT_i->TYPE == VARIABLE_CLASS) || (_CONTEXT_i->TYPE == VARIABLE_DELEGATE))) {
                            _CONTEXT_i->TYPE       = VARIABLE_NUMBER;
                            _CONTEXT_i->CLASS_DATA = 0;
                        }
                        //=======================================//
                        _CONTEXT_i->LINKS--;
                        if (_CONTEXT_i->LINKS < 1) {
                            if (_CONTEXT_i->CLASS_DATA) {
                                if (_CONTEXT_i->TYPE == VARIABLE_STRING) {
                                    delete (AnsiString *)_CONTEXT_i->CLASS_DATA;
                                } else
                                if ((_CONTEXT_i->TYPE == VARIABLE_CLASS) || (_CONTEXT_i->TYPE == VARIABLE_DELEGATE)) {
                                    if (!--((CompiledClass *)_CONTEXT_i->CLASS_DATA)->LINKS) {
                                        if (inspectPos >= inspectSize) {
                                            if (inspectSize < 0xFFFF)
                                                inspectSize += INSPECT_INCREMENT;
                                            else
                                                inspectSize *= 1.5;
                                            toInspect = (CompiledClass **)FAST_REALLOC(toInspect, sizeof(CompiledClass *) * inspectSize);
                                        }
                                        toInspect[inspectPos++] = (CompiledClass *)_CONTEXT_i->CLASS_DATA;
                                    }
                                } else
                                if (_CONTEXT_i->TYPE == VARIABLE_ARRAY) {
                                    if (!--((Array *)_CONTEXT_i->CLASS_DATA)->LINKS)
                                        delete (Array *)_CONTEXT_i->CLASS_DATA;
                                }
                            }
                            VAR_FREE(_CONTEXT_i);
                        }
                    }
                }
                FAST_FREE(obj->_CONTEXT);
                obj->_CONTEXT = NULL;
            }
            delete obj;
        }
        FAST_FREE(toInspect);
    }
#endif
}

/*
	The Balsa Asynchronous Hardware Synthesis System
	Copyright (C) 1995-2009 School of Computer Science
	The University of Manchester, Oxford Road, Manchester, UK, M13 9PL
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

	`parts.c'
	Functions/Types to aid in recovery of procedures from parts
	
 */

#include "parts.h"
#include "Errors.h"
#include "operators.h"
#include "ports.h"
#include <string.h>
#include <ctype.h>

/* ...Ident : tIdent hashings of part attribute keywords */
static tIdent IsProcedureIdent = 0;
static tIdent IsFunctionIdent = 0;
static tIdent IsPermanentIdent = 0;
static tIdent IsExternalIdent = 0;
static tIdent LineIdent = 0;
static tIdent ColumnIdent = 0;

tIdent IsBuiltinFunctionIdent = 0;

/*
   Parameter types

   type boolean is enumeration false, true end

   type UnaryOperator is enumeration    -- UnaryFunc operation type
   Negate, Invert
   end

   type BinaryOperator is enumeration    -- BinaryFunc operation type
   Add, Subtract, ReverseSubtract, Equals, NotEquals,
   LessThan, GreaterThan, LessOrEquals,
   GreaterOrEquals, And, Or
   end
 */

#define BOOLEAN_IDENT_COUNT (2)
tIdent FalseIdent = 0;          /* Idents for false, true, ... are based from this ident */

/* BeginParts : initialise this module, apres BeginIdents */
void BeginParts (void)
{
    IsProcedureIdent = MakeIdent1 ("is-procedure");
    IsFunctionIdent = MakeIdent1 ("is-function");
    IsBuiltinFunctionIdent = MakeIdent1 ("is-builtin-function");
    IsPermanentIdent = MakeIdent1 ("is-permanent");
    IsExternalIdent = MakeIdent1 ("is-external");
    LineIdent = MakeIdent1 ("line");
    ColumnIdent = MakeIdent1 ("column");

    FalseIdent = MakeIdent1 ("false");
    MakeIdent1 ("true");
}

/* NewAttributedProcedure : make a new procedure and populate it with the given 
   part information (ident and attributes) */
PtrProcedure NewAttributedProcedure (tIdent ident, PtrLispList attributes, unsigned portCount, tPosition position)
{
    PtrProcedure ret = NULL;

    if (!attributes)
        return NULL;

    if (RemoveHeadedLispListElement (&attributes, IsProcedureIdent, NULL))
        ret = NewProcedure (ident, UnSharedProcedure, portCount, position);
    else if (RemoveHeadedLispListElement (&attributes, IsFunctionIdent, NULL))
        ret = NewProcedure (ident, UnSharedFunction, portCount, position);
    else if (RemoveHeadedLispListElement (&attributes, IsBuiltinFunctionIdent, NULL))
        ret = NewProcedure (ident, BuiltinFunction, portCount, position);
    else
    {
        LOG_ERROR (FirstPartAttributeMustBeIsSomething, ident, position);
        return NULL;
    }

    /* Now handle the rest of the attributes */
    if (RemoveHeadedLispListElement (&attributes, IsExternalIdent, NULL))
        ret->isExternal = true;
    if (RemoveHeadedLispListElement (&attributes, IsPermanentIdent, NULL))
        ret->info.procedure.attributes.permanent = true;

    /* Should have removed all the known attributes now, commit the rest to the procedure attributes */
    ret->userAttributes = attributes;

    return ret;
}

/* HandleParameter : handle to correct placement of a parameter into a component,
   set actualSpec to a string of possible types for the object eg. "#i" for IntegerParameter,
   paramNo is the parameter number (left->right, based at 0).
   If `parameters' is not NULL then use that as the place to hold the parameters values */
void
HandleParameter (Ptrchar actualSpec, unsigned paramNo, PtrComponent component,
  void *actualValue, PtrContext context, PtrComponentParameterList parameters, tPosition position)
{
    PtrComponentSpec spec;
    char specChar;

    /* Fail silently */
    if (!component)
        return;

    spec = BreezeComponents + (int) component->nature;
    specChar = spec->paramSpec[paramNo];

    /* Lots of error checks */
    if (component->nature == ProcedureComponent)
    {
        PtrComponentParameter param = NthOfComponentParameterList (parameters, paramNo);

        if (!param)
            LOG_ERROR (TypeIsNotValid, NoIdent, position);
        else
        {
            if (strchr (actualSpec, 't'))
            {
                if (param->nature != TypeComponentParameter)
                    LOG_ERROR (TypeIsNotValid, NoIdent, position);
                else
                    param->value.type = actualValue;
            } else if (strchr (actualSpec, '$'))
            {
                if (param->nature != StringComponentParameter)
                {
                    LOG_ERROR (TypeIsNotValid, NoIdent, position);
                } else
                    param->value.string = (Ptrchar) PeekString ((tIdent) actualValue);
            } else if (strchr (actualSpec, 'i'))
            {
                if (param->nature != NumberComponentParameter)
                    LOG_ERROR (TypeIsNotValid, NoIdent, position);
                else
                    param->value.number = actualValue;
            }
        }
    } else if (paramNo > MAX_BREEZE_PARAMETER_COUNT || paramNo > strlen (spec->paramSpec))
        LOG_ERROR (TooManyParameters, NoIdent, position);
    else if (!strchr (actualSpec, specChar))
    {
        LOG_ERROR (TypeIsNotValid, NoIdent, position);
    } else
    {
        switch (specChar)
        {
        case 'i':              /* MP_INT */
            if (spec->offsets[paramNo])
                *((PtrMP_INT *) (((Ptrchar) component) + spec->offsets[paramNo])) = (PtrMP_INT) actualValue;
            break;
        case '#':              /* int */
            {
                int value = mpz_get_si ((PtrMP_INT) actualValue);

                if (spec->offsets[paramNo])
                    *((int *) (((Ptrchar) component) + spec->offsets[paramNo])) = value;
            }
            break;
        case '$':              /* string */
            if (spec->offsets[paramNo])
                *((Ptrchar *) (((Ptrchar) component) + spec->offsets[paramNo])) = (Ptrchar) PeekString ((tIdent) actualValue);
            break;
        case 'b':              /* bool */
            if (((tIdent) actualValue) != FalseIdent && ((tIdent) actualValue) != FalseIdent + 1)
                LOG_ERROR (TypeIsNotValid, NoIdent, position);
            else
                *((bool *) (((Ptrchar) component) + spec->offsets[paramNo])) = ((tIdent) actualValue) - FalseIdent;
            break;
        case 'B':              /* Operator (binary) */
            {
                Operators op = FindBinaryOperator ((tIdent) actualValue, true);

                if (op == InvalidOperator)
                    LOG_ERROR (TypeIsNotValid, NoIdent, position);
                else
                    *((Operators *) (((Ptrchar) component) + spec->offsets[paramNo])) = op;
            }
            break;
        case 'U':              /* Operators (unary) */
            {
                Operators op = FindUnaryOperator ((tIdent) actualValue, true);

                if (op == InvalidOperator)
                    LOG_ERROR (TypeIsNotValid, NoIdent, position);
                else
                    *((Operators *) (((Ptrchar) component) + spec->offsets[paramNo])) = op;
            }
            break;
        case 't':              /* Type */
            *((PtrType *) (((Ptrchar) component) + spec->offsets[paramNo])) = (PtrType) actualValue;
            break;
        default:
            ASSERT (false);     /* Can't happen */
            break;
        }
    }
}

/* NewComponentFromComponentName : create a new component from the given ident,
   make a $BrzWhatEver component if the ident is valid and internal is true, 
   otherwise lookup the name as a procedure in context and make a ProcedureComponent */
PtrComponent NewComponentFromComponentName (PtrContext context, tIdent ident, bool internal, tPosition position)
{
    PtrComponent ret = NULL;

    if (internal)
    {
        if (ident >= FirstBreezeComponentIdent && ident < (FirstBreezeComponentIdent + LastComponent))
            ret = NewComponent (NoPosition, (ComponentNature) (ident - FirstBreezeComponentIdent));
        else
            LOG_ERROR (InvalidBreezeComponent, ident, position);
    } else
        /* Procedure component */
    {
        PtrProcedure procedure = LookupProcedure (context, ident, false /* Not just local */ );

        if (procedure == NoProcedure)
            LOG_ERROR (UnboundIdentifier, ident, position);
        else
        {
            ret = NewComponent (NoPosition, ProcedureComponent);
            ret->param.procedure.ident = ident;
            ret->param.procedure.ports = procedure->ports;
            ret->param.procedure.portSpec = procedure->portSpec;
        }
    }
    return ret;
}

/* UpdateWiresForComponent : update the component/portNo links in the given wire array from
	the wire connections to a new component.  Returns a list of port wires for the component. */
PtrWireList UpdateWiresFromComponent (PtrComponent component, PtrintList channelNumbers, PtrWireArray wires, tPosition position)
{
    Ptrchar spec = GetComponentPortSpec (component);
    PtrWireList ret = NULL;

    /* portCounter is used to count down for port arrays */
    int portCounter = -1;
    unsigned portNo = 0;

    while (channelNumbers)
    {
        PtrWire wire;
        char specChar;

        /* End of spec ? */
        if (*spec == '\0')
        {
            LOG_ERROR (ExprListTooLong, NoIdent, CAR (channelNumbers).position);
            return NULL;
        }

        wire = wires[CAR (channelNumbers).value - 1].body;
        /* Quit on unresolves wires */
        if (!wire)
        {
            LOG_ERROR (WireNotDeclared, NoIdent, CAR (channelNumbers).position);
            return NULL;
        }

        /* Arrayed port ? */
        if (portCounter != -1)
        {
            specChar = *spec;
            portCounter--;
        } else
        {
            specChar = *spec;
            spec++;
        }

        /* Start of array ? */
        if (isdigit (specChar)) /* beware of [0-9][0-9][isSIoO] */
        {
            portCounter = GetComponentIntegerParameterAtPosition (component, specChar - '0') - 1;
            /* Skip over count digit */
            specChar = *spec;
        }

        /* Set the sense of the selected wire (in the wire structure, according to the spec) */

        switch (specChar)
        {
        case 'i':
            UpdateWire (wire, false, component, portNo);
            wire->isPull = false;
            break;
        case 'I':
            UpdateWire (wire, true, component, portNo);
            wire->isPull = true;
            break;
        case 'o':
            UpdateWire (wire, false, component, portNo);
            wire->isPull = true;
            break;
        case 'O':
            UpdateWire (wire, true, component, portNo);
            wire->isPull = false;
            break;
        case 's':
            UpdateWire (wire, false, component, portNo);
            break;
        case 'S':
            UpdateWire (wire, true, component, portNo);
            break;
        default:
            break;
        }
        /* End of array ? */
        if (portCounter == 0)
        {
            portCounter = -1;
            spec++;
        }

        ret = AppendWireLists (ret, NewWireList (wire, NULL));
        portNo++;

        channelNumbers = CDR (channelNumbers);
    }
    /* Any spec left ? */
    if (*spec != '\0')
    {
        if (component->nature == PassiveEagerFalseVariableComponent)
            return ret;
        if (component->nature == ActiveEagerFalseVariableComponent)
            return ret;

        if (component->nature == ProcedureComponent)
        {
            fprintf (stderr, "%s %s %d\n", GetComponentPortSpec (component), spec, portNo);
            WriteIdent (stderr, component->param.procedure.ident);
            fprintf (stderr, "\n");
        }
        LOG_ERROR (ExprListTooShort, NoIdent, position);
    }

    return ret;
}

/* HandleComp : handle resolution of component bundles (parsing breeze),
   add component to incoming list if all is OK */
PtrComponentList
HandleComp (PtrComponentList list, PtrComponent component, PtrintList channelNumbers, PtrWireArray wires, tPosition position, PtrLispList options)
{
    PtrComponentList ret = list;
    PtrWireList componentWires = NULL;

    component->options = options;
    component->position = position;

    componentWires = UpdateWiresFromComponent (component, channelNumbers, wires, position);

    if (componentWires)
    {
        component->ports = componentWires;
        ret = NewComponentList (component, list);
    }

    return ret;
}

/* HandleUndeclaredComp : handle an undeclared component and all the extra port/parameter
	complexity that entails */
PtrComponentList
HandleUndeclaredComp (PtrComponentList list, PtrComponent component,
  PtrintList channelNumbers, PtrWireArray wires,
  tPosition position, tIdent componentType,
  tIdent baseComponentName, PtrComponentParameterList parameters, PtrInstanceList ports, PtrLispList options)
{
    PtrComponentList ret = list;
    PtrWireList componentWires = NULL;

    //  Ptrchar spec = NULL;  /* GetComponentPortSpec (component); */

    component->options = options;
    component->position = position;
    component->param.procedure.ports = ports;
    component->param.procedure.parameters = parameters;
    component->param.procedure.portSpec = MakePortSpecString (ports, LengthOfInstanceList (ports));
    component->param.procedure.componentType = componentType;
    component->param.procedure.baseComponentName = baseComponentName;

    /* Need to make a port spec! */

    componentWires = UpdateWiresFromComponent (component, channelNumbers, wires, position);

    if (componentWires)
    {
        component->ports = componentWires;
        ret = NewComponentList (component, list);
    }

    return ret;
}

/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the V4VM module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QString>
#include "debugging.h"
#include <qv4context.h>
#include <qv4object.h>
#include <qv4objectproto.h>
#include "qv4mm.h"
#include <qv4argumentsobject.h>

namespace QQmlJS {
namespace VM {

DiagnosticMessage::DiagnosticMessage()
    : offset(0)
    , length(0)
    , startLine(0)
    , startColumn(0)
    , type(0)
    , next(0)
{}

DiagnosticMessage::~DiagnosticMessage()
{
    delete next;
}

String *DiagnosticMessage::buildFullMessage(ExecutionContext *ctx) const
{
    QString msg;
    if (!fileName.isEmpty())
        msg = fileName + QLatin1Char(':');
    msg += QString::number(startLine) + QLatin1Char(':') + QString::number(startColumn) + QLatin1String(": ");
    if (type == QQmlJS::VM::DiagnosticMessage::Error)
        msg += QLatin1String("error");
    else
        msg += QLatin1String("warning");
    msg += ": " + message;

    return ctx->engine->newString(msg);
}

DEFINE_MANAGED_VTABLE(ExecutionContext);

void ExecutionContext::createMutableBinding(String *name, bool deletable)
{
    if (!activation)
        activation = engine->newObject();

    if (activation->__hasProperty__(this, name))
        return;
    PropertyDescriptor desc;
    desc.value = Value::undefinedValue();
    desc.type = PropertyDescriptor::Data;
    desc.configurable = deletable ? PropertyDescriptor::Enabled : PropertyDescriptor::Disabled;
    desc.writable = PropertyDescriptor::Enabled;
    desc.enumerable = PropertyDescriptor::Enabled;
    activation->__defineOwnProperty__(this, name, &desc);
}

bool ExecutionContext::setMutableBinding(ExecutionContext *scope, String *name, const Value &value)
{
    // ### throw if scope->strict is true, and it would change an immutable binding
    if (function) {
        for (unsigned int i = 0; i < function->varCount; ++i)
            if (function->varList[i]->isEqualTo(name)) {
                locals[i] = value;
                return true;
            }
        for (int i = (int)function->formalParameterCount - 1; i >= 0; --i)
            if (function->formalParameterList[i]->isEqualTo(name)) {
                arguments[i] = value;
                return true;
            }
    }

    if (activation && (qmlObject || activation->__hasProperty__(scope, name))) {
        activation->put(scope, name, value);
        return true;
    }

    return false;
}

Value ExecutionContext::getBindingValue(ExecutionContext *scope, String *name, bool strict) const
{
    Q_UNUSED(strict);
    assert(function);

    if (function) {
        for (unsigned int i = 0; i < function->varCount; ++i)
            if (function->varList[i]->isEqualTo(name))
                return locals[i];
        for (int i = (int)function->formalParameterCount - 1; i >= 0; --i)
            if (function->formalParameterList[i]->isEqualTo(name))
                return arguments[i];
    }

    if (activation) {
        bool hasProperty = false;
        Value v = activation->get(scope, name, &hasProperty);
        if (hasProperty)
            return v;
    }
    assert(false);
}

bool ExecutionContext::deleteBinding(ExecutionContext *scope, String *name)
{
    if (activation)
        activation->deleteProperty(scope, name);

    if (scope->strictMode)
        scope->throwTypeError();
    return false;
}

String * const *ExecutionContext::formals() const
{
    return function ? function->formalParameterList : 0;
}

unsigned int ExecutionContext::formalCount() const
{
    return function ? function->formalParameterCount : 0;
}

String * const *ExecutionContext::variables() const
{
    return function ? function->varList : 0;
}

unsigned int ExecutionContext::variableCount() const
{
    return function ? function->varCount : 0;
}


void ExecutionContext::init(ExecutionEngine *eng)
{
    engine = eng;
    outer = 0;
    thisObject = eng->globalObject;

    function = 0;
    lookups = 0;

    arguments = 0;
    argumentCount = 0;
    locals = 0;
    exceptionVarName = 0;
    exceptionValue = Value::undefinedValue();
    strictMode = false;
    qmlObject = false;
    activation = 0;
    withObject = 0;
}

void ExecutionContext::init(ExecutionContext *p, Object *with)
{
    engine = p->engine;
    outer = p;
    thisObject = p->thisObject;

    function = 0;
    lookups = p->lookups;

    arguments = 0;
    argumentCount = 0;
    locals = 0;
    exceptionVarName = 0;
    exceptionValue = Value::undefinedValue();
    strictMode = false;
    qmlObject = false;
    activation = 0;
    withObject = with;
}

void ExecutionContext::initForCatch(ExecutionContext *p, String *exceptionVarName, const Value &exceptionValue)
{
    engine = p->engine;
    outer = p;
    thisObject = p->thisObject;

    function = 0;
    lookups = p->lookups;
    arguments = 0;
    argumentCount = 0;
    locals = 0;
    this->exceptionVarName = exceptionVarName;
    this->exceptionValue = exceptionValue;
    strictMode = p->strictMode;
    qmlObject = false;
    activation = 0;
    withObject = 0;
}

bool ExecutionContext::deleteProperty(String *name)
{
    bool hasWith = false;
    for (ExecutionContext *ctx = this; ctx; ctx = ctx->outer) {
        if (ctx->withObject) {
            hasWith = true;
            if (ctx->withObject->__hasProperty__(this, name))
                return ctx->withObject->deleteProperty(this, name);
        } else {
            if (ctx->activation && ctx->activation->__hasProperty__(this, name))
                return ctx->activation->deleteProperty(this, name);
        }
        if (ctx->exceptionVarName && ctx->exceptionVarName->isEqualTo(name))
            return false;
        if (FunctionObject *f = ctx->function) {
            if (f->needsActivation || hasWith) {
                for (unsigned int i = 0; i < f->varCount; ++i)
                    if (f->varList[i]->isEqualTo(name))
                        return false;
                for (int i = (int)f->formalParameterCount - 1; i >= 0; --i)
                    if (f->formalParameterList[i]->isEqualTo(name))
                        return false;
            }
        }
    }
    if (strictMode)
        throwSyntaxError(0);
    return true;
}

void ExecutionContext::destroy(Managed *that)
{
    ExecutionContext *ctx = static_cast<ExecutionContext *>(that);
    if (ctx->locals)
        delete [] ctx->locals;
    ctx->_data = 0;
    ctx->vtbl = 0;
}

void ExecutionContext::markObjects(Managed *that)
{
    ExecutionContext *ctx = static_cast<ExecutionContext *>(that);
    ctx->thisObject.mark();
    if (ctx->function)
        ctx->function->mark();
    for (unsigned arg = 0, lastArg = ctx->formalCount(); arg < lastArg; ++arg)
        ctx->arguments[arg].mark();
    for (unsigned local = 0, lastLocal = ctx->variableCount(); local < lastLocal; ++local)
        ctx->locals[local].mark();
    if (ctx->activation)
        ctx->activation->mark();
    if (ctx->withObject)
        ctx->withObject->mark();
    if (ctx->exceptionVarName)
        ctx->exceptionVarName->mark();
    ctx->exceptionValue.mark();
}

Value ExecutionContext::get(Managed *m, ExecutionContext *ctx, String *name, bool *hasProperty)
{
    Q_UNIMPLEMENTED();
    Q_UNREACHABLE();
}

Value ExecutionContext::getIndexed(Managed *m, ExecutionContext *ctx, uint index, bool *hasProperty)
{
    Q_UNIMPLEMENTED();
    Q_UNREACHABLE();
}

void ExecutionContext::put(Managed *m, ExecutionContext *ctx, String *name, const Value &value)
{
}

void ExecutionContext::putIndexed(Managed *m, ExecutionContext *ctx, uint index, const Value &value)
{
}

PropertyFlags ExecutionContext::query(Managed *m, ExecutionContext *ctx, String *name)
{
    Q_UNIMPLEMENTED();
    Q_UNREACHABLE();
}

PropertyFlags ExecutionContext::queryIndexed(Managed *m, ExecutionContext *ctx, uint index)
{
    Q_UNIMPLEMENTED();
    Q_UNREACHABLE();
}

bool ExecutionContext::deleteProperty(Managed *m, ExecutionContext *ctx, String *name)
{
    Q_UNIMPLEMENTED();
    Q_UNREACHABLE();
}

bool ExecutionContext::deleteIndexedProperty(Managed *m, ExecutionContext *ctx, uint index)
{
    Q_UNIMPLEMENTED();
    Q_UNREACHABLE();
}

void ExecutionContext::setProperty(String *name, const Value& value)
{
//    qDebug() << "=== SetProperty" << value.toString(this)->toQString();
    for (ExecutionContext *ctx = this; ctx; ctx = ctx->outer) {
        if (Object *w = ctx->withObject) {
//            qDebug() << ctx << "hasWith";
            if (w->__hasProperty__(ctx, name)) {
//                qDebug() << "   withHasProp";
                w->put(ctx, name, value);
                return;
            }
        } else if (ctx->exceptionVarName && ctx->exceptionVarName->isEqualTo(name)) {
            ctx->exceptionValue = value;
            return;
        } else {
//            qDebug() << ctx << "setting mutable binding";
            if (ctx->setMutableBinding(this, name, value))
                return;
        }
    }
    if (strictMode || name->isEqualTo(engine->id_this))
        throwReferenceError(Value::fromString(name));
    engine->globalObject.objectValue()->put(this, name, value);
}

Value ExecutionContext::getProperty(String *name)
{
    name->makeIdentifier(this);

    if (name->isEqualTo(engine->id_this))
        return thisObject;

    bool hasWith = false;
    bool hasCatchScope = false;
//    qDebug() << "=== getProperty" << name->toQString();
    for (ExecutionContext *ctx = this; ctx; ctx = ctx->outer) {
        if (Object *w = ctx->withObject) {
            hasWith = true;
//            qDebug() << ctx << "hasWith";
            bool hasProperty = false;
            Value v = w->get(ctx, name, &hasProperty);
            if (hasProperty) {
//                qDebug() << "   withHasProp";
                return v;
            }
            continue;
        }

        if (ctx->exceptionVarName) {
            hasCatchScope = true;
            if (ctx->exceptionVarName->isEqualTo(name))
                return ctx->exceptionValue;
        }

        if (FunctionObject *f = ctx->function) {
            if (f->needsActivation || hasWith || hasCatchScope) {
                for (unsigned int i = 0; i < f->varCount; ++i)
                    if (f->varList[i]->isEqualTo(name))
                        return ctx->locals[i];
                for (int i = (int)f->formalParameterCount - 1; i >= 0; --i)
                    if (f->formalParameterList[i]->isEqualTo(name))
                        return ctx->arguments[i];
            }
        }
        if (ctx->activation) {
            bool hasProperty = false;
            Value v = ctx->activation->get(ctx, name, &hasProperty);
            if (hasProperty)
                return v;
        }
        if (FunctionObject *f = ctx->function) {
            if (f->function && f->function->isNamedExpression
                && name->isEqualTo(f->function->name))
                return Value::fromObject(ctx->function);
        }
    }
    throwReferenceError(Value::fromString(name));
    return Value::undefinedValue();
}

Value ExecutionContext::getPropertyNoThrow(String *name)
{
    name->makeIdentifier(this);

    if (name->isEqualTo(engine->id_this))
        return thisObject;

    bool hasWith = false;
    bool hasCatchScope = false;
    for (ExecutionContext *ctx = this; ctx; ctx = ctx->outer) {
        if (Object *w = ctx->withObject) {
            hasWith = true;
            bool hasProperty = false;
            Value v = w->get(ctx, name, &hasProperty);
            if (hasProperty)
                return v;
            continue;
        }

        if (ctx->exceptionVarName) {
            hasCatchScope = true;
            if (ctx->exceptionVarName->isEqualTo(name))
                return ctx->exceptionValue;
        }

        if (FunctionObject *f = ctx->function) {
            if (f->needsActivation || hasWith || hasCatchScope) {
                for (unsigned int i = 0; i < f->varCount; ++i)
                    if (f->varList[i]->isEqualTo(name))
                        return ctx->locals[i];
                for (int i = (int)f->formalParameterCount - 1; i >= 0; --i)
                    if (f->formalParameterList[i]->isEqualTo(name))
                        return ctx->arguments[i];
            }
        }
        if (ctx->activation) {
            bool hasProperty = false;
            Value v = ctx->activation->get(ctx, name, &hasProperty);
            if (hasProperty)
                return v;
        }
        if (FunctionObject *f = ctx->function) {
            if (f->function && f->function->isNamedExpression
                && name->isEqualTo(f->function->name))
                return Value::fromObject(ctx->function);
        }
    }
    return Value::undefinedValue();
}

Value ExecutionContext::getPropertyAndBase(String *name, Object **base)
{
    *base = 0;
    name->makeIdentifier(this);

    if (name->isEqualTo(engine->id_this))
        return thisObject;

    bool hasWith = false;
    bool hasCatchScope = false;
    for (ExecutionContext *ctx = this; ctx; ctx = ctx->outer) {
        if (Object *w = ctx->withObject) {
            hasWith = true;
            bool hasProperty = false;
            Value v = w->get(ctx, name, &hasProperty);
            if (hasProperty) {
                *base = w;
                return v;
            }
            continue;
        }

        if (ctx->exceptionVarName) {
            hasCatchScope = true;
            if (ctx->exceptionVarName->isEqualTo(name))
                return ctx->exceptionValue;
        }

        if (FunctionObject *f = ctx->function) {
            if (f->needsActivation || hasWith || hasCatchScope) {
                for (unsigned int i = 0; i < f->varCount; ++i)
                    if (f->varList[i]->isEqualTo(name))
                        return ctx->locals[i];
                for (int i = (int)f->formalParameterCount - 1; i >= 0; --i)
                    if (f->formalParameterList[i]->isEqualTo(name))
                        return ctx->arguments[i];
            }
        }
        if (ctx->activation) {
            bool hasProperty = false;
            Value v = ctx->activation->get(ctx, name, &hasProperty);
            if (hasProperty)
                return v;
        }
        if (FunctionObject *f = ctx->function) {
            if (f->function && f->function->isNamedExpression
                && name->isEqualTo(f->function->name))
                return Value::fromObject(ctx->function);
        }
    }
    throwReferenceError(Value::fromString(name));
    return Value::undefinedValue();
}



void ExecutionContext::inplaceBitOp(String *name, const Value &value, BinOp op)
{
    Value lhs = getProperty(name);
    Value result;
    op(this, &result, lhs, value);
    setProperty(name, result);
}

void ExecutionContext::throwError(const Value &value)
{
    __qmljs_builtin_throw(this, value);
}

void ExecutionContext::throwError(const QString &message)
{
    Value v = Value::fromString(this, message);
    throwError(Value::fromObject(engine->newErrorObject(v)));
}

void ExecutionContext::throwSyntaxError(DiagnosticMessage *message)
{
    throwError(Value::fromObject(engine->newSyntaxErrorObject(this, message)));
}

void ExecutionContext::throwTypeError()
{
    throwError(Value::fromObject(engine->newTypeErrorObject(this, QStringLiteral("Type error"))));
}

void ExecutionContext::throwUnimplemented(const QString &message)
{
    Value v = Value::fromString(this, QStringLiteral("Unimplemented ") + message);
    throwError(Value::fromObject(engine->newErrorObject(v)));
}

void ExecutionContext::throwReferenceError(Value value)
{
    String *s = value.toString(this);
    QString msg = s->toQString() + QStringLiteral(" is not defined");
    throwError(Value::fromObject(engine->newReferenceErrorObject(this, msg)));
}

void ExecutionContext::throwRangeError(Value value)
{
    String *s = value.toString(this);
    QString msg = s->toQString() + QStringLiteral(" out of range");
    throwError(Value::fromObject(engine->newRangeErrorObject(this, msg)));
}

void ExecutionContext::throwURIError(Value msg)
{
    throwError(Value::fromObject(engine->newURIErrorObject(this, msg)));
}

void ExecutionContext::initCallContext(ExecutionEngine *engine)
{
    this->engine = engine;
    outer = function->scope;

    exceptionVarName = 0;
    exceptionValue = Value::undefinedValue();

    activation = 0;
    withObject = 0;

    strictMode = function->strictMode;
    qmlObject = false;

    if (function->function)
        lookups = function->function->lookups;

    uint argc = argumentCount;
    uint valuesToAlloc = function->varCount;
    bool copyArgs = function->needsActivation || argumentCount < function->formalParameterCount;
    if (copyArgs)
        valuesToAlloc += qMax(argc, function->formalParameterCount);

    if (valuesToAlloc) {
        locals = new Value[valuesToAlloc];
        if (function->varCount)
            std::fill(locals, locals + function->varCount, Value::undefinedValue());

        if (copyArgs) {
            Value *args = arguments;
            argumentCount = qMax(argc, function->formalParameterCount);
            arguments = locals + function->varCount;
            if (argc)
                ::memcpy(arguments, args, argc * sizeof(Value));
            if (argc < function->formalParameterCount)
                std::fill(arguments + argc, arguments + function->formalParameterCount, Value::undefinedValue());

        }
    }
    if (function->usesArgumentsObject) {
        ArgumentsObject *args = new (engine->memoryManager) ArgumentsObject(this, function->formalParameterCount, argc);
        args->prototype = engine->objectPrototype;
        Value arguments = Value::fromObject(args);
        createMutableBinding(engine->id_arguments, false);
        setMutableBinding(this, engine->id_arguments, arguments);
    }

    if (engine->debugger)
        engine->debugger->aboutToCall(function, this);
}

void ExecutionContext::wireUpPrototype()
{
    assert(thisObject.isObject());

    Value proto = function->get(this, engine->id_prototype);
    if (proto.isObject())
        thisObject.objectValue()->prototype = proto.objectValue();
    else
        thisObject.objectValue()->prototype = engine->objectPrototype;
}

} // namespace VM
} // namespace QQmlJS

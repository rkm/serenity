/*
 * Copyright (c) 2020, Stephan Unverwerth <s.unverwerth@gmx.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <AK/Function.h>
#include <LibJS/AST.h>
#include <LibJS/Interpreter.h>
#include <LibJS/Runtime/Error.h>
#include <LibJS/Runtime/ScriptFunction.h>
#include <LibJS/Runtime/Value.h>

namespace JS {

ScriptFunction::ScriptFunction(const FlyString& name, const Statement& body, Vector<FlyString> parameters, LexicalEnvironment* parent_environment)
    : m_name(name)
    , m_body(body)
    , m_parameters(move(parameters))
    , m_parent_environment(parent_environment)
{
    put("prototype", heap().allocate<Object>());
    put_native_property("length", length_getter, length_setter);
}

ScriptFunction::~ScriptFunction()
{
}

void ScriptFunction::visit_children(Visitor& visitor)
{
    Function::visit_children(visitor);
    visitor.visit(m_parent_environment);
}

LexicalEnvironment* ScriptFunction::create_environment()
{
    HashMap<FlyString, Variable> variables;
    for (auto& parameter : m_parameters) {
        variables.set(parameter, { js_undefined(), DeclarationKind::Var });
    }

    if (body().is_scope_node()) {
        for (auto& declaration : static_cast<const ScopeNode&>(body()).variables()) {
            for (auto& declarator : declaration.declarations()) {
                variables.set(declarator.id().string(), { js_undefined(), DeclarationKind::Var });
            }
        }
    }
    if (variables.is_empty())
        return m_parent_environment;
    return heap().allocate<LexicalEnvironment>(move(variables), m_parent_environment);
}

Value ScriptFunction::call(Interpreter& interpreter)
{
    auto& argument_values = interpreter.call_frame().arguments;
    ArgumentVector arguments;
    for (size_t i = 0; i < m_parameters.size(); ++i) {
        auto name = parameters()[i];
        auto value = js_undefined();
        if (i < argument_values.size())
            value = argument_values[i];
        arguments.append({ name, value });
        interpreter.current_environment()->set(name, { value, DeclarationKind::Var });
    }
    return interpreter.run(m_body, arguments, ScopeType::Function);
}

Value ScriptFunction::construct(Interpreter& interpreter)
{
    return call(interpreter);
}

Value ScriptFunction::length_getter(Interpreter& interpreter)
{
    auto* this_object = interpreter.this_value().to_object(interpreter.heap());
    if (!this_object)
        return {};
    if (!this_object->is_function())
        return interpreter.throw_exception<TypeError>("Not a function");
    return Value(static_cast<i32>(static_cast<const ScriptFunction*>(this_object)->parameters().size()));
}

void ScriptFunction::length_setter(Interpreter&, Value)
{
}

}

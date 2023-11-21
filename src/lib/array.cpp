#include <sstream>
#include <stdexcept>
#include <memory>
#include <cmath>
#include <variant>
#include "infixparser.h"
#include "array.h"

ArrayVariable::ArrayVariable(const std::string& varName, std::unique_ptr<ASTNode> index)
    : arrayName(varName), index(std::move(index)) {}

double ArrayVariable::evaluate(std::map<std::string, std::variant<double, std::vector<double>>>& symbolTable) const {
    double indexValue = index->evaluate(symbolTable); //og map is just string, double -> new map has string and variant of doubel and array of doubles

    auto it = symbolTable.find(arrayName);
    if (it != symbolTable.end()) {
        if (std::holds_alternative<std::vector<double>>(it->second)) {
            const std::vector<double>& array = std::get<std::vector<double>>(it->second);

            double fractionalPart;
            if (modf(indexValue, &fractionalPart) == 0.0) {
                int arrayIndex = static_cast<int>(indexValue);
                if (arrayIndex >= 0 && arrayIndex < static_cast<int>(array.size())) {
                    return array[arrayIndex];
                } else {
                    throw OutOfBoundsError();
                }
            } else {
                throw IntegerIndexError();
            }
        } else if (std::holds_alternative<double>(it->second)) {
            if (indexValue == 0.0) {
                return std::get<double>(it->second);
            } else {
                throw OutOfBoundsError(); 
            }
        } else {
            throw NotAnArrayError();
        }
    } else {
        throw NotAnArrayError();
    }
}

std::string ArrayVariable::toInfix() const {
    std::stringstream ss;
    ss << arrayName << "[";
    ss << index->toInfix();
    ss << "]";
    return ss.str();
}

// ArrayAssignment 
ArrayAssignment::ArrayAssignment(const std::string& varName, std::unique_ptr<ASTNode> index, std::unique_ptr<ASTNode> expression)
    : arrayName(varName), index(std::move(index)), expression(std::move(expression)) {}

double ArrayAssignment::evaluate(std::map<std::string, std::variant<double, std::vector<double>>>& symbolTable) const {
    double indexValue = index->evaluate(symbolTable);

    auto it = symbolTable.find(arrayName);
    if (it != symbolTable.end()) {
        if (std::holds_alternative<std::vector<double>>(it->second)) {
            std::vector<double>& array = std::get<std::vector<double>>(it->second);
            double fractionalPart;
            if (modf(indexValue, &fractionalPart) == 0.0) {
                int arrayIndex = static_cast<int>(indexValue);

                // Check if the array index is within bounds
                if (arrayIndex >= 0 && arrayIndex < static_cast<int>(array.size())) {
                    double expressionValue = expression->evaluate(symbolTable);
                    array[arrayIndex] = expressionValue;
                    return expressionValue;
                } else {
                    throw OutOfBoundsError();
                }
            } else {
                throw IntegerIndexError();
            }
        } else {
            throw NotAnArrayError();
        }
    } else {
        throw NotAnArrayError();
    }
}

std::string ArrayAssignment::toInfix() const {
    std::stringstream ss;
    ss << arrayName << "[";
    ss << index->toInfix();
    ss << "] = ";
    ss << expression->toInfix();
    return ss.str();
}

// ArrayIndex 
ArrayIndex::ArrayIndex(std::unique_ptr<ASTNode> array, std::unique_ptr<ASTNode> index)
    : array(std::move(array)), index(std::move(index)) {}

double ArrayIndex::evaluate(std::map<std::string, std::variant<double, std::vector<double>>>& symbolTable) const {
    double arrayValue = array->evaluate(symbolTable);

    // Check if the variable is an array
    if (std::holds_alternative<std::vector<double>>(arrayValue)) {
        const std::vector<double>& array = std::get<std::vector<double>>(arrayValue);
        double indexValue = index->evaluate(symbolTable);

        double fractionalPart;
        if (modf(indexValue, &fractionalPart) == 0.0) {
            int arrayIndex = static_cast<int>(indexValue);

            // Check if the array index is within bounds
            if (arrayIndex >= 0 && arrayIndex < static_cast<int>(array.size())) {
                return array[arrayIndex];
            } else {
                throw OutOfBoundsError();
            }
        } else {
            throw IntegerIndexError();
        }
    } else if (std::holds_alternative<double>(arrayValue)) {
        if (indexValue == 0.0) { //*********
            return std::get<double>(arrayValue);
        } else {
            throw OutOfBoundsError(); 
        }
    } else {
        throw NotAnArrayError();
    }
}
std::string ArrayIndex::toInfix() const {
    std::stringstream ss;
    ss << array->toInfix() << "[";
    ss << index->toInfix();
    ss << "]";
    return ss.str();
}

std::variant<double, std::vector<double>> ArrayLiteral::evaluate(std::map<std::string, std::variant<double, std::vector<double>>>& symbolTable) const {
    std::vector<double> evaluatedElements;

    for (const auto& element : elements) {
        auto elementValue = element->evaluate(symbolTable);
        // Assuming elementValue is always convertible to double
        evaluatedElements.push_back(std::get<double>(elementValue));
    }

    return evaluatedElements;
}

std::string ArrayLiteral::toInfix() const {
        std::string result = "[";

    for (size_t i = 0; i < elements.size(); ++i) {
        result += elements[i]->toInfix();
        if (i < elements.size() - 1) {
            result += ", ";
        }
    }

    result += "]";
    return result;
}

// Utility functions
double len(const std::vector<double>& array) {
    return static_cast<double>(array.size());
}

double pop(std::vector<double>& array) {
    if (array.empty()) {
        throw UnderError();
    }
    double poppedValue = array.back();
    array.pop_back();
    return poppedValue;
}

double push(std::vector<double>& array, double value) {
    array.push_back(value);
    return 0.0; // should return null ******
}
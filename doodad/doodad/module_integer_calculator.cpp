#include "module_integer_calculator.h"
#include <vector>
#include <stack>


enum class EType : uint32_t
{
	Expression,
	Operand,
	Operator
};

enum class EOperation : uint32_t
{
	Exponent = 0,
	Multiply,
	Divide,
	Modulo,
	Add,
	Subtract,
	ShiftLeft,
	ShiftRight,
	And,
	XOr,
	Or,
	None,
};


class IntExpElement
{
public:

	EType mType = EType::Expression;
	inttype mValue = 0;
	EOperation mOp = EOperation::None;

	std::vector<IntExpElement> mElements;

	inttype EvalOperator(IntExpElement* left, IntExpElement* right, EOperation op)
	{
		inttype leftVal = left->mValue;
		inttype rightVal = right->mValue;
		inttype outVal = INFINITY;

		switch (op)
		{
		case EOperation::Exponent: outVal = pow(leftVal, rightVal); break;
		case EOperation::Multiply: outVal = leftVal * rightVal; break;
		case EOperation::Divide: outVal = leftVal / rightVal; break;
		case EOperation::Add: outVal = leftVal + rightVal; break;
		case EOperation::Subtract: outVal = leftVal - rightVal; break;
		case EOperation::And: outVal = leftVal & rightVal; break;
		case EOperation::Or: outVal = leftVal | rightVal; break;
		case EOperation::ShiftLeft: outVal = leftVal << rightVal; break;
		case EOperation::ShiftRight: outVal = leftVal >> rightVal; break;
		case EOperation::XOr: outVal = leftVal ^ rightVal; break;
		case EOperation::Modulo: outVal = leftVal % rightVal; break;
		}

		return outVal;
	}

	IntExpElement Eval()
	{
		if (mType == EType::Operand || mType == EType::Operator)
		{
			IntExpElement copy;
			copy.mElements = mElements;
			copy.mType = mType;
			copy.mValue = mValue;
			copy.mOp = mOp;
			return copy;
		}

		//First, eval all expressions, create new element array with expressions collapsed to doubles
		std::vector<IntExpElement> NoExpressions;
		for (IntExpElement e : mElements)
		{
			NoExpressions.push_back(e.Eval());
		}

		//after evaluating subexpressions, you should end up with a NoExpressions array like this: 
		// Operand -> Operator -> Operand -> Operator -> Operand
		// 7 * 33 + 90

		//Now iterate over all operands in order of priority
		EOperation OpToProcess = (EOperation)0;

		while (OpToProcess != EOperation::None)
		{
			for (int i = 0; i < NoExpressions.size(); ++i)
			{
				IntExpElement e = NoExpressions[i];
				if (e.mType == EType::Operator && e.mOp == OpToProcess)
				{
					inttype result = EvalOperator(&NoExpressions[i - 1], &NoExpressions[i + 1], e.mOp);

					NoExpressions[i - 1].mValue = result;
					NoExpressions[i - 1].mType = EType::Operand;

					NoExpressions.erase(NoExpressions.begin() + i, NoExpressions.begin() + i + 2);
					i = 0;
				}
			}

			int nextOp = (int)OpToProcess;
			OpToProcess = (EOperation)(++nextOp);
		}

		//the final result should be a single operand, which is the value of this expression. 

		IntExpElement result;
		result.mType = EType::Operand;
		result.mValue = NoExpressions[0].mValue;

		return result;
	}
};

bool ModuleIntegerCalculator::CanHandleInput(std::string input) const
{
	auto IsIntegerNumeric = [this](std::string input)
	{
		return IsHex(input) || IsInteger(input) || IsBinary(input);
	};

	auto IsIntegerOrBrace = [this, &IsIntegerNumeric](std::string input)
	{
		return IsIntegerNumeric(input) || input == "(" || input == ")";
	};

	std::vector<std::string> delims;

	delims.push_back("(");
	delims.push_back(")");
	delims.push_back("&");
	delims.push_back(">>");
	delims.push_back("<<");
	delims.push_back("|");
	delims.push_back("#");
	delims.push_back("+");
	delims.push_back("-");
	delims.push_back("*");
	delims.push_back("^");
	delims.push_back("/");
	delims.push_back("%");

	std::vector<std::string> tokens = TokenizeString(input, delims);
	bool lastTokenWasOperator = false;
	bool allIntegerOrDelim = true;
	uint32_t numOpenBraces = 0;
	uint32_t numCloseBraces = 0;

	for (int i = 0; i < tokens.size(); ++i)
	{
		if (std::find(delims.begin(), delims.end(), tokens[i]) == delims.end())
		{
			lastTokenWasOperator = false;
			if (!IsIntegerNumeric(tokens[i]))
			{
				allIntegerOrDelim = false;
				break;
			}
		}
		else
		{
			if (lastTokenWasOperator && !IsIntegerOrBrace(tokens[i])) return false;
			lastTokenWasOperator = !IsIntegerOrBrace(tokens[i]);
			if (tokens[i] == "(") numOpenBraces++;
			if (tokens[i] == ")") numCloseBraces++;
		}
	}

	return tokens.size() > 1 && allIntegerOrDelim && (numOpenBraces == numCloseBraces) && IsIntegerOrBrace(tokens[tokens.size() - 1]);
}

std::string ModuleIntegerCalculator::HandleInput(std::string input) const
{
	auto IsIntegerNumeric = [this](std::string input)
	{
		return IsHex(input) || IsDecimal(input) || IsBinary(input);
	};

	auto IsIntegerOrBrace = [this, &IsIntegerNumeric](std::string input)
	{
		return IsIntegerNumeric(input) || input == "(" || input == ")";
	};

	std::vector<std::string> delims;
	delims.push_back("(");
	delims.push_back(")");
	delims.push_back("^");
	delims.push_back("*");
	delims.push_back("/");
	delims.push_back("%");
	delims.push_back("+");
	delims.push_back("-");
	delims.push_back("<<");
	delims.push_back(">>");
	delims.push_back("&");
	delims.push_back("#");
	delims.push_back("|");

	std::vector<std::string> tokens = TokenizeString(input, delims);
	ERadix outputRadix = ERadix::Invalid;
	bool hasCommonRadix = true;

	for (int i = 0; i < tokens.size(); ++i)
	{
		if (IsIntegerNumeric(tokens[i]))
		{
			ERadix r = GetRadix(tokens[i]);
			if (outputRadix == ERadix::Invalid)
			{
				outputRadix = r;
			}
			else if (r != outputRadix)
			{
				hasCommonRadix = false;
				break;
			}

		}
	}
	if (!hasCommonRadix)
	{
		outputRadix = ERadix::Decimal;
	}

	IntExpElement rootExpression;
	rootExpression.mType = EType::Expression;

	std::stack<IntExpElement> expressionToProcess; //index of the expression being parsed right now. -1 == root expression
	expressionToProcess.push(rootExpression);

	for (int i = 0; i < tokens.size(); ++i)
	{
		std::string& token = tokens[i];

		if (token == "(")
		{
			IntExpElement subExpression;
			subExpression.mType = EType::Expression;
			expressionToProcess.push(subExpression);
		}
		else if (token == ")")
		{
			IntExpElement subExpression = expressionToProcess.top();
			expressionToProcess.pop();
			expressionToProcess.top().mElements.push_back(subExpression);

		}
		else if (std::find(delims.begin() + 2, delims.end(), token) != delims.end())
		{
			auto iter = std::find(delims.begin() + 2, delims.end(), token);

			IntExpElement newOperator;
			newOperator.mType = EType::Operator;

			switch (iter - delims.begin())
			{
			case 2: newOperator.mOp = EOperation::Exponent; break;
			case 3: newOperator.mOp = EOperation::Multiply; break;
			case 4: newOperator.mOp = EOperation::Divide; break;
			case 5: newOperator.mOp = EOperation::Modulo; break;
			case 6: newOperator.mOp = EOperation::Add; break;
			case 7: newOperator.mOp = EOperation::Subtract; break;
			case 8: newOperator.mOp = EOperation::ShiftLeft; break;
			case 9: newOperator.mOp = EOperation::ShiftRight; break;
			case 10: newOperator.mOp = EOperation::And; break;
			case 11: newOperator.mOp = EOperation::XOr; break;
			case 12: newOperator.mOp = EOperation::Or; break;
			}

			expressionToProcess.top().mElements.push_back(newOperator);
		}
		else
		{
			IntExpElement newOperand;
			newOperand.mType = EType::Operand;
			newOperand.mValue = NumericToInt(token);

			expressionToProcess.top().mElements.push_back(newOperand);

		}
	}

	IntExpElement result = expressionToProcess.top().Eval();
	inttype resVal = result.mValue;
	
	return  input + " = " + IntToRadixString(resVal, outputRadix);
}

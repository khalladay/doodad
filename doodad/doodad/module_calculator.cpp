#include "module_calculator.h"
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
	Add,
	Subtract,
	None,
};


static class ExpElement
{
public:

	EType mType = EType::Expression;
	float mValue = 0.0f;
	EOperation mOp = EOperation::None;

	std::vector<ExpElement> mElements;

	float EvalOperator(ExpElement* left, ExpElement* right, EOperation op)
	{
		float leftVal = left->mValue;
		float rightVal = right->mValue;
		float outVal = INFINITY;
		switch (op)
		{
		case EOperation::Exponent: outVal = pow(leftVal, rightVal); break;
		case EOperation::Multiply: outVal = leftVal * rightVal; break;
		case EOperation::Divide: outVal = leftVal / rightVal; break;
		case EOperation::Add: outVal = leftVal + rightVal; break;
		case EOperation::Subtract: outVal = leftVal - rightVal; break;
		}

		return outVal;
	}

	ExpElement Eval()
	{
		if (mType == EType::Operand || mType == EType::Operator)
		{
			ExpElement copy;
			copy.mElements = mElements;
			copy.mType = mType;
			copy.mValue = mValue;
			copy.mOp = mOp;
			return copy;
		}

		//First, eval all expressions, create new element array with expressions collapsed to doubles
		std::vector<ExpElement> NoExpressions;
		for (ExpElement e : mElements)
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
				ExpElement e = NoExpressions[i];
				if (e.mType == EType::Operator && e.mOp == OpToProcess)
				{
					float result = EvalOperator(&NoExpressions[i - 1], &NoExpressions[i + 1], e.mOp);

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

		ExpElement result;
		result.mType = EType::Operand;
		result.mValue = NoExpressions[0].mValue;

		return result;
	}
};



bool ModuleCalculator::CanHandleInput(std::string input) const
{
	auto IsNumericOrBrace = [this](std::string input)
	{
		return IsDecimal(input) || input == "(" || input == ")";
	};

	std::vector<std::string> delims;
	delims.push_back("*");
	delims.push_back("/");
	delims.push_back("+");
	delims.push_back("-");
	delims.push_back("(");
	delims.push_back(")");
	delims.push_back("^");

	std::vector<std::string> tokens = TokenizeString(input, delims);

	bool allNumericsAreInts = true;

	bool allNumericOrDelim = true;
	bool lastTokenWasOperator = false;
	uint32_t numOpenBraces = 0;
	uint32_t numCloseBraces = 0;
	for (int i = 0; i < tokens.size(); ++i)
	{
		if (std::find(delims.begin(), delims.end(), tokens[i]) == delims.end())
		{
			lastTokenWasOperator = false;
			if (!IsDecimal(tokens[i]))
			{
				allNumericOrDelim = false;
				break;
			}
			else
			{
				if (!IsInteger(tokens[i]))
				{
					allNumericsAreInts = false;
				}
			}
		}
		else
		{
			if (lastTokenWasOperator && !IsNumericOrBrace(tokens[i])) return false;
			lastTokenWasOperator = !IsNumericOrBrace(tokens[i]);
			if (tokens[i] == "(") numOpenBraces++;
			if (tokens[i] == ")") numCloseBraces++;
		}
	}
	return tokens.size() > 1 && !allNumericsAreInts && allNumericOrDelim && (numOpenBraces == numCloseBraces) && IsNumericOrBrace(tokens[tokens.size()-1]);
}

std::string ModuleCalculator::HandleInput(std::string input) const
{
	std::vector<std::string> delims;
	delims.push_back("(");
	delims.push_back(")");
	delims.push_back("*");
	delims.push_back("/");
	delims.push_back("+");
	delims.push_back("-");
	delims.push_back("^");
	std::vector<std::string> tokens = TokenizeString(input, delims);


	ExpElement rootExpression;
	rootExpression.mType = EType::Expression;

	std::stack<ExpElement> expressionToProcess; //index of the expression being parsed right now. -1 == root expression
	expressionToProcess.push(rootExpression);

	for (int i = 0; i < tokens.size(); ++i)
	{
		std::string& token = tokens[i];

		if (token == "(")
		{
			ExpElement subExpression;
			subExpression.mType = EType::Expression;
			expressionToProcess.push(subExpression);
		}
		else if (token == ")")
		{
			ExpElement subExpression = expressionToProcess.top();
			expressionToProcess.pop();
			expressionToProcess.top().mElements.push_back(subExpression);

		}
		else if (std::find(delims.begin() + 2, delims.end(), token) != delims.end())
		{
			auto iter = std::find(delims.begin() + 2, delims.end(), token);
		
			ExpElement newOperator;
			newOperator.mType = EType::Operator;

			switch(iter - delims.begin())
			{
			case 2: newOperator.mOp = EOperation::Multiply; break;
			case 3: newOperator.mOp = EOperation::Divide; break;
			case 4: newOperator.mOp = EOperation::Add; break;
			case 5: newOperator.mOp = EOperation::Subtract; break;
			case 6: newOperator.mOp = EOperation::Exponent; break;
			}

			expressionToProcess.top().mElements.push_back(newOperator);
		}
		else
		{
			ExpElement newOperand;
			newOperand.mType = EType::Operand;
			newOperand.mValue = DecimalToFloat(token);

			expressionToProcess.top().mElements.push_back(newOperand);

		}
	}

	ExpElement result = expressionToProcess.top().Eval();
	float resVal = result.mValue;
	return input + " = " + std::to_string(resVal);
}
/*
 * Copyright (c) 2016 Stefan Wallentowitz <wallento@silicon-semantics.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "IR.h"

#include <stdexcept>

namespace Firrtlator {

typedef enum { MALE, FEMALE, BI } Gender;

Expression::Expression() : mGender(MALE) {}

Expression::Expression(Gender g) : mGender(g) {}

Expression::Gender Expression::getGender() {
	return mGender;
}

Reference::Reference() : Reference("") {}

Reference::Reference(std::string id) : mId(id), mTo(nullptr) {}

bool Reference::isResolved() {
	return (mTo != nullptr && mTo != 0);
}

void Reference::accept(Visitor& v) {

}

Constant::Constant() : Constant(nullptr, -1, UNDEFINED) {}

Constant::Constant(std::shared_ptr<TypeInt> type, int val,
		GenerateHint hint)
: mType(type), mVal(val), mHint(hint) {}

Constant::Constant(std::shared_ptr<TypeInt> type, std::string val,
		GenerateHint hint)
: mType(type), mHint(hint) {
	mVal = 0; //TODO: convert
}

void Constant::accept(Visitor& v) {

}

SubField::SubField() : SubField(nullptr, nullptr) {}

SubField::SubField(std::shared_ptr<Reference> id, std::shared_ptr<Expression> of)
: mId(id), mOf(of) {}

void SubField::accept(Visitor& v) {

}

SubIndex::SubIndex() : SubIndex(-1, nullptr) {}

SubIndex::SubIndex(int index, std::shared_ptr<Expression> of)
: mIndex(index), mOf(of) {}

void SubIndex::accept(Visitor& v) {

}

SubAccess::SubAccess() : SubAccess(nullptr, nullptr) {}

SubAccess::SubAccess(std::shared_ptr<Reference> expr,
		std::shared_ptr<Expression> of)
: mOf(of), mExp(expr) {}

void SubAccess::accept(Visitor& v) {

}

Mux::Mux() : Mux(nullptr, nullptr, nullptr) {}

Mux::Mux(std::shared_ptr<Expression> sel, std::shared_ptr<Expression> a,
		std::shared_ptr<Expression> b)
: mSel(sel), mA(a), mB(b) {}

void Mux::accept(Visitor& v) {

}

CondValid::CondValid() : CondValid(nullptr, nullptr) {}

CondValid::CondValid(std::shared_ptr<Expression> sel,
		std::shared_ptr<Expression> a)
: mSel(sel), mA(a) {}

void CondValid::accept(Visitor& v) {

}

const bool PrimOp::lookup(std::string v, Operation &op) {
	static const std::map<std::string, Operation> map =	{
		{"add", ADD}, {"sub", SUB }, {"mul", MUL },
		{"div", DIV}, {"mod", MOD }, {"lt", LT },
		{"leq", LEQ}, {"gt", GT}, {"geq", GEQ }, {"eq", EQ},
	    {"neq", NEQ}, {"pad", PAD}, {"asUInt", ASUINT},
		{"asSInt", ASSINT}, {"asClock", ASCLOCK},
		{"shl", SHL}, {"shr", SHR}, {"dshl", DSHL},
		{"dshr", DSHR}, {"cvt", CVT}, {"neg", NEG},
		{"not", NOT}, {"and", AND}, {"or", OR},
		{"xor", XOR}, {"andr", ANDR}, {"orr", ORR},
		{"xorr", XORR}, {"cat", CAT}, {"bits", BITS},
		{"head", HEAD}, {"tail", TAIL}
	};

	if (map.find(v) == map.end()) {
		op = UNDEFINED;
		return false;
	}

	op = map.find(v)->second;
	return true;
}

PrimOp::PrimOp() : PrimOp(UNDEFINED, 0, 0) {}

PrimOp::PrimOp(Operation op, int numOps, int numParams) :
		Expression(MALE), mOp(op),
		mNumOperands(numOps), mNumParameters(numParams) {

}

std::shared_ptr<PrimOp> PrimOp::generate(const Operation &op) {
#define GENOP(x) case x: return std::make_shared<PrimOp##x>();
	switch (op) {
	GENOP(ADD);	GENOP(SUB);	GENOP(MUL);	GENOP(DIV);	GENOP(MOD);
	GENOP(LT); GENOP(LEQ); GENOP(GT); GENOP(GEQ); GENOP(EQ);
	GENOP(NEQ);	GENOP(PAD);	GENOP(ASUINT);GENOP(ASSINT); GENOP(ASCLOCK);
	GENOP(SHL);	GENOP(SHR);	GENOP(DSHL); GENOP(DSHR); GENOP(CVT);
	GENOP(NEG);	GENOP(NOT);	GENOP(AND);	GENOP(OR); GENOP(XOR);
	GENOP(ANDR); GENOP(ORR); GENOP(XORR); GENOP(CAT); GENOP(BITS);
	GENOP(HEAD); GENOP(TAIL);
	default:
		throw std::runtime_error("Unknown operation type");
	}
#undef GENOP
}

const std::string PrimOp::operationName(Operation op) {
#define GENOP(x,y) case x: return y;
	switch(op) {
	GENOP(ADD,"add"); GENOP(SUB,"sub"); GENOP(MUL,"mul"); GENOP(DIV,"div");
	GENOP(MOD,"mod"); GENOP(LT,"lt"); GENOP(LEQ,"leq"); GENOP(GT,"gt");
	GENOP(GEQ,"geq"); GENOP(EQ,"eq"); GENOP(NEQ,"neq"); GENOP(PAD,"pad");
	GENOP(ASUINT,"asUInt"); GENOP(ASSINT,"asSInt"); GENOP(ASCLOCK,"asClock");
	GENOP(SHL,"shl"); GENOP(SHR,"shr"); GENOP(DSHL,"dshl"); GENOP(DSHR,"dshr");
	GENOP(CVT,"cvt"); GENOP(NEG,"neg"); GENOP(NOT,"not"); GENOP(AND,"and");
	GENOP(OR,"or"); GENOP(XOR,"xor"); GENOP(ANDR,"andr"); GENOP(ORR,"orr");
	GENOP(XORR,"xorr"); GENOP(CAT,"cat"); GENOP(BITS,"bits");
	GENOP(HEAD,"head"); GENOP(TAIL,"tail");
	default:
		throw std::runtime_error("Unknown operation type");
	}
#undef GENOP
}

std::string PrimOp::operationName() {
	return operationName(mOp);
}

std::shared_ptr<PrimOp> PrimOp::generate(const std::string &s) {
	Operation op;
	if (!lookup(s, op)) {
		throw std::runtime_error("Unknown operation type");
	}

	return generate(op);
}

void PrimOp::addOperand(std::shared_ptr<Expression> o) {
	if (mOperands.size() == mNumOperands) {
		throw std::runtime_error("Too many operands");
	}
	mOperands.push_back(o);
}

void PrimOp::addParameter(int p) {
	if (mParameters.size() == mNumParameters) {
		throw std::runtime_error("Too many parameters");
	}
	mParameters.push_back(p);
}

void PrimOp::accept(Visitor& v) {

}

}
#ifndef LEXER_HPP_
#define LEXER_HPP_

#include "../include/lexer.hpp"

namespace fsmc {

class FSM {
	public:
		FSM ();

		FSM &next (char c);

		bool is_running () const
		{
			return m_state != 255 && m_state != 254;
		}

		uint8_t get_matched () const
		{
			return m_last;
		}

		bool is_match_state () const;
			
	private:
		uint8_t m_last;
		uint8_t m_state;

		struct __jump {
			__jump (uint8_t s, uint8_t a = 0, uint8_t aa = 0);
			uint8_t next_state;
			uint8_t action;
			uint8_t action_arg;
		} **m_jump_table;
};
	
}
class FSM_compiler {
	public:
		using fsm_type = FSM;

		FSM_compiler () = default;

		template <class Cont>
		fsm_type make (Cont &&tokens) {return FSM ();}
};

template <class IStreamT>
class Lexer : public gla::Lexer <gla::Token_maker, FSM_compiler,
    gla::lexis <uint8_t, std::string>, IStreamT> {
	public:
		Lexer (IStreamT *in) : gla::Lexer <Token_maker, FSM_compiler,
		    gla::Lexis <uint8_t, std::string>, IStreamT> (in)
		{}
};

} // end of fsmc namespace

#endif // LEXER_HPP_

#include "../include/lexer.hpp"
#include "../include/token_maker.hpp"
#include <memory>
#include <iostream>

class FSM {
	public:
		FSM () : m_state (0)
		{
		}

		bool is_running () const
		{
			return m_state != 255;
		}

		template <class CharT>
		FSM &next (CharT &&c)
		{
			switch (m_state) {
			case 0:
				if (c =='#') {
					m_state = 2;
				} else if (c == EOF) {
					m_state = 5;
				} else {
					m_state = 1;
				}
				m_last.push_back (0);
				break;
			case 1:
				m_state = 255;
				break;
			case 2:
				if (c == 'i') {
					m_state = 3;
				} else {
					m_state = 255;
				}
				break;
			case 3:
				if (c == 'f') {
					m_last.clear ();
					m_state = 4;
					m_last.push_back (1);
				} else {
					m_state = 255;
				}
				break;
			case 4:
				m_state = 255;
				break;
			case 5:
				m_last.clear ();
				m_last.push_back (EOF);
				m_state = 255;
				break;
			}

			return *this;
		}

		void reset ()
		{
			m_state = 0;
			m_last.clear ();
		}

		bool is_match_state () const
		{
			switch (m_state) {
			case 1: case 5: case 4:
				return true;
			default:
				return false;
			}
		}

		decltype (auto) get_matched () const
		{
			return *m_last.begin ();
		}
	private:
		uint64_t m_state;
		std::vector <int> m_last;
};

class FSM_compiler {
	public:
		using fsm_type = FSM;
		using pattern_type = std::basic_string <char>;
		FSM_compiler () = default;

		template <class T>
		fsm_type make (T &&patterns) {

			return FSM ();
		}
};

template <class TokenTagT, class InputIterator>
std::basic_ostream <char> &
operator<< (std::basic_ostream <char> &out,
    const gla::Token <TokenTagT, InputIterator> &tok)
{
	if (tok.tag == 0) {
		out << tok.lexeme;
	} else if (tok.tag == 1) {
		out << "\033[32m" << tok.lexeme << "\033[0m";
	}

	return out;
}

int main () {
	gla::Lexer <gla::Token_maker,
	       FSM_compiler,
		   gla::Lexis <int, std::basic_string <char> >,
		   std::basic_istream <char> > lex (&std::cin, gla::Lexis <int, std::basic_string <char> > ());

	for (auto token = lex.scan (); token.tag != EOF; token = lex.scan ())
	{
		//auto token = lex.scan ();
		std::cout << token; 
	}

	return 0;
}

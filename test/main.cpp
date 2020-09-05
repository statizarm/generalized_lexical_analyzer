#include "../include/lexer.hpp"
#include <memory>
#include <iostream>

class Token {
	public:
		template <class T>
		Token (int tag, T&& beg, T&& end) : m_tag (tag), m_str (beg, end)
		{ }

		friend std::basic_ostream <char>
		&operator<< (std::basic_ostream <char> &out, const Token &tok);

		int tag () const
		{
			return m_tag;
		}
	private:
		int m_tag;
		std::basic_string <char> m_str;
};

std::basic_ostream <char> &
operator<< (std::basic_ostream <char> &out, const Token &tok)
{
	if (tok.m_tag == 0) {
		out << tok.m_str;
	} else if (tok.m_tag == 1) {
		out << /*"\033[32m" <<*/ tok.m_str/* << "\033[0m"*/;
	}

	return out;
}

class Token_maker {
	public:
		using token_type = Token;
		using token_id_type = int;
		Token_maker () = default;

		template <class ContainerIterator,
		          class InputIterator>
		decltype (auto)
		make_tokens (ContainerIterator cont_begin,
		    ContainerIterator cont_end,
			InputIterator in_begin,
			InputIterator in_end)
		{
			if (cont_begin == cont_end) {
				std::cerr << "empty matched tokens container" << std::endl;
				exit (1);
			}

			if (in_begin == in_end) {
				std::cout << "empty token lexeme" << std::endl;
			}

			if (*in_begin == 0) {
				std::cout << "lexeme containes zero code symbol" << std::endl;
			}

			return Token (*cont_begin, in_begin, in_end);
		}
};

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

		const std::vector <int> &get_matched () const
		{
			return m_last;
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
		fsm_type make_fsm (T &&patterns) {

			return FSM ();
		}
};

using namespace gla;

int main () {
	Lexer <Token_maker, FSM_compiler, std::basic_istream <char> > lex (&std::cin, Lexis <int, std::basic_string <char> > ());

	for (auto token = lex.scan (); token.tag () != EOF; token = lex.scan ())
	{
		//auto token = lex.scan ();
		std::cout << token; 
	}

	return 0;
}

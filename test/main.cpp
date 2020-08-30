#include "../include/lexer.hpp"
#include <memory>
#include <iostream>

class Token {
	public:
		template <class T>
		Token (int tag, T&& beg, T&& end) : m_tag (tag), m_str (beg, end)
		{ }

		friend std::basic_ostream <wchar_t>
		&operator<< (std::basic_ostream <wchar_t> &out, const Token &tok);

		int tag () const
		{
			return m_tag;
		}
	private:
		int m_tag;
		std::basic_string <wchar_t> m_str;
};

std::basic_ostream <wchar_t> &
operator<< (std::basic_ostream <wchar_t> &out, const Token &tok)
{
	if (tok.m_tag == 0) {
		out << tok.m_str;
	} else if (tok.m_tag == 1) {
		out << "\033[32m" << tok.m_str << "\033[0m";
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
			std::vector <Token> res;

			while (cont_begin != cont_end) {
				res.emplace_back (*cont_begin, in_begin, in_end);
			}

			return res;
		}
};

class FSM {
	public:
		FSM () : m_state (0)
		{
		}

		bool is_running () const
		{
			if (m_state != 255) {
				return false;
			} else {
				return true;
			}
		}

		template <class CharT>
		void next (CharT &&c)
		{
			switch (m_state) {
			case 0:
				if (c =='#') {
					m_state = 2;
				} else {
					m_state = 1;
				}
				break;
			case 1:
				m_last.clear ();
				m_state = 255;
				m_last.emplace_back (0);
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
					m_last.emplace_back (1);
					m_state = 4;
				} else {
					m_state = 255;
				}
			case 4:
				m_last.clear ();
				m_last.emplace_back (1);
				m_state = 255;
				break;
			}
		}

		void reset ()
		{
			m_state = 0;
			m_last.clear ();
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
		using pattern_type = std::basic_string <wchar_t>;
		FSM_compiler () = default;

		template <class T>
		fsm_type make_fsm (T &&patterns) {

			return FSM ();
		}
};

using namespace gla;

int main () {
	Lexer <Token_maker, FSM_compiler> lex (&std::wcin, Lexis <int, std::basic_string <wchar_t> > ());

	lex.scan ();

	return 0;
}

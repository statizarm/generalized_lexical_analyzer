#ifndef GLA_LEXER_H_
#define GLA_LEXER_H_

#include <ostream>

namespace gla {

template <class TokenMakerT,
          class FSMCompilerT,
		  class InputStreamT = std::basic_buffer <wchar_t> >
class Lexer {
	public:
		using token_type    = TokenMaker::token_type;
		using token_id_type = TokenMaker::token_id_type;
		using fsm_type      = FSMCompilerT::fsm_type;
		using pattern_type  = FSMCompilerT::pattern_type;
		using char_type     = InputStreamT::char_type;

		using fsm_compiler_type = FSMCompilerT;
		using token_maker_type  = TokenMakerT;

		using istream_type = InputStreamT;

		Lexer () = delete;
		Lexer (istream_type &in,
			   const Lexis <token_id_type, pattern_type> &lex)
		  : m_machines (),
		    m_buff (in)
		{
			for (auto &&[token_id, token_pattern] : lex) {
				auto &&fsm = fsm_compiler_type::compile (token_pattern);

				m_machines.emplace_back (token_id, fsm);
			}
		}

		Lexer (const Lexer &) = delete;
		
		token_type scan ()
		{
			std::list <std::reference <machine_type> > running_machines (
			    m_machines.begin (), m_machines.end ());

			auto start_it = m_buff.get_pos ();

			auto last_matched = std::make_pair (token_id_type (), start_it);

			char_type c = *start_it;

			while (running_machines.size () > 0) {
				auto mach_it = running_machines.begin ();
				auto end     = running_machines.end ();

				while (mach_it != end) {
					auto [tok_id, mach] = *mach_it;

					mach.next (c);

					if (mach.ismatched ()) {
						last_matched.first = tok_id;
						last_matched.second = m_buff.curr_pos () + 1;
					}

					if (!mach.isrunning ()) {
						running_machines.remove (mach_it);
					} else {
						++mach_it;
					}
				}

				c = m_buff.getchar ();
			}

			m_buff.set_pos (last_matched.second);
			return token_maker_type::make_token (last_matched.first,
			                                     start_it,
												 last_matched.second);
		}
	protected:
		using machine_type = std::pair <token_id_type, fsm_type>;
		std::vector <machine_type> m_machines;
	private:
		Lexer_buffer <char_type> m_buff;
};

} // End gla namespace

#endif // GLA_LEXER_H_

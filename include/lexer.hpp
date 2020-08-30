#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <vector>
#include <utility>
#include <istream>
#include <iterator>
#include <list>
#include "lexer_buffer.hpp"
#include "lexis.hpp"

namespace gla {

template <class TokenMakerT,
          class FSMCompilerT,
          class IStreamT = std::basic_istream <wchar_t> >
class Lexer {
	public:
		using token_maker_type = TokenMakerT;
		using fsm_compiler_type = FSMCompilerT;
		using istream_type = IStreamT;

		using token_type = typename token_maker_type::token_type;
		using token_id_type = typename token_maker_type::token_id_type;
		using pattern_type = typename fsm_compiler_type::pattern_type;
		using fsm_type   = typename fsm_compiler_type::fsm_type;

		template <class CompT = FSMCompilerT,
		          class MakerT = TokenMakerT>
		Lexer (IStreamT *in,
               const Lexis <token_id_type, pattern_type> &lexis,
			   CompT &&fsm_comp = fsm_compiler_type (),
			   MakerT &&token_maker = token_maker_type ())
		  : m_fsm (fsm_comp.make_fsm (lexis.get_tokens ())),
		    m_token_maker (token_maker), m_buff (in)
		{
		}

		decltype (auto) scan ()
		{
			m_fsm.reset ();
			
			auto beg = m_buff.getpos ();
  
			auto c = (m_buff.is_empty ()) ? m_buff.getchar () : *beg;

			while (true) {
				m_fsm.next (c);

				if (!m_fsm.is_running ()) {
					break;
				}

				c = getchar ();
			}

			auto end = m_buff.getpos ();
			
			auto &&matched = m_fsm.get_matched ();

			return m_token_maker.make_tokens (std::begin (matched),
			    std::end (matched), beg, end);
		}
	protected:
		fsm_type m_fsm;
		token_maker_type m_token_maker;
	private:
		implementation_details::Lexer_buffer <istream_type> m_buff;
};

} // end of gla namespace

#endif // LEXER_HPP_

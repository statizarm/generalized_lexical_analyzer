#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <vector>
#include <pair>
#include <istream>
#include <iterator>
#include <algorithm>
#include <functional>
#include <list>
#include "lexer_buffer.hpp"

namespace gla {

template <class TokenMakerT,
          class FSMCompilerT,
          class IStreamT = std::basic_istream <wchar_t> >
class Lexer {
	public:
		using token_maker_type = TokenMakerT;
		using fsm_compiler_type = FSMCompilerT;
		using istream_type = IStreamT;

		using token_type = token_maker_type::token_type;
		using token_id_type = token_maker_type::token_id_type;
		using fsm_type   = fsm_compiler_type::fsm_type;

		Lexer (istream_type *in,
               const Lexis <token_id_type, pattern_type> &lexis)
		  : m_fsm (fsm_compiler_type::make_fsm (lexis.get_tokens ()),
            m_buff (in)
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
			
			auto matched = m_fsm.get_matched ();

			return token_maker_type::make_tokens (std::begin (matched),
			    std::end (matched), beg, end);
		}
	protected:
		fsm_type m_fsm;
	private:
		implementation_details::Lexer_buffer <istream_type> m_buff;
};

} // end of gla namespace

#endif // LEXER_HPP_

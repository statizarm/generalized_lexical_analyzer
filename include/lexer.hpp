#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <iostream>
#include <iostream>
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
		  class LexisT = gla::Lexis <int, std::basic_string <wchar_t> >,
          class IStreamT = std::basic_istream <wchar_t> >
class Lexer {
	public:
		using token_maker_type  = TokenMakerT;
		using fsm_compiler_type = FSMCompilerT;
		using lexis_type        = LexisT;
		using istream_type      = IStreamT;

		using fsm_type = typename fsm_compiler_type::fsm_type;

		template <class CompT  = FSMCompilerT,
		          class MakerT = TokenMakerT,
				  class LexT   = LexisT>
		Lexer (IStreamT *in, LexT &&lexis,
			   CompT &&fsm_comp = fsm_compiler_type (),
			   MakerT &&token_maker = token_maker_type ())
		  : m_fsm (fsm_comp.make (lexis.get_tokens ())),
		    m_token_maker (token_maker), m_buff (new buffer (in))
		{
		}

		~Lexer ()
		{
			delete m_buff;
		}

		decltype (auto) scan ()
		{
			m_fsm.reset ();
			
			auto c = m_buff->peek ();
			
			auto begin   = m_buff->getpos ();
			auto forward = begin;

			while (m_fsm.next (c).is_running ()) {

				if (m_fsm.is_match_state ()) {
					forward = m_buff->getpos ();
				}

				c = m_buff->getchar ();
			}

			m_buff->setpos (++forward);

			return m_token_maker.make (m_fsm.get_matched (), begin, forward);
		}
	protected:
		fsm_type m_fsm;
		token_maker_type m_token_maker;
	private:
		using buffer = implementation_details::Lexer_buffer <istream_type>;
		buffer *m_buff;
};

} // end of gla namespace

#endif // LEXER_HPP_

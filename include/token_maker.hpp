#ifndef GLA_TOKEN_MAKER_T_
#define GLA_TOKEN_MAKER_T_

#include "lexer_buffer.hpp"
#include <type_traits>

namespace gla {

template <class T>
struct remove_cvref {
	using type = std::remove_reference_t <std::remove_cv_t <T> >;
};
	
template <class TokenT>
struct Token_traits {
	using tag_type = typename TokenT::tag_type;
	using lexeme_type = typename TokenT::lexeme_type;
};

template <class TokenTagT,
          class InputIterator>
struct Token {
	using char_type = typename std::iterator_traits <InputIterator>::value_type;
	using lexeme_type = std::basic_string <char_type>;

	template <class TagT = TokenTagT>
	Token (TagT &&id, InputIterator begin, InputIterator end)
	  : tag (id), lexeme (begin, end)
	{ }

	TokenTagT tag;
	lexeme_type lexeme;
};

class Token_maker {
	public:
		Token_maker () = default;
		template <class TokenTagT,
		          class InputIterator>
		decltype (auto) make (TokenTagT token_id,
		    InputIterator begin,
			InputIterator end)
		{
			return Token <typename remove_cvref <TokenTagT>::type,
			    typename remove_cvref <InputIterator>::type>
				(token_id, begin, end);
		}
};

template <class TokenTagT, class InputIterator>
struct Token_traits <Token <TokenTagT, InputIterator> > {
	using tag_type = TokenTagT;
	using lexeme_type = std::basic_string
	    <typename std::iterator_traits <InputIterator>::value_type>;
};

} // end of gla namespace


#endif // GLA_TOKEN_MAKER_T_


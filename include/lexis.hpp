#ifndef LEXIS_HPP_
#define LEXIS_HPP_

namespace gla {

template <class TokenIDT,
          class PatternT>
class Lexis {
	public:
		using token_id_type = TokenIDT;
		using pattern_type  = PatternT;

		using token_type = std::pair <token_id_type, pattern_type>;
		using container_type = std::vector <token_type>;

		Lexis () = default;

		void add_token (const token_id_type &id, const pattern_type &pattern)
		{
			m_tokens.emplace_back (id, pattern);
		}
		
		const container_type &get_tokens () const
		{
			return m_tokens;
		}
	private:
		container_type m_tokens;
};

} // end of gla namespace

#endif // LEXIS_HPP_


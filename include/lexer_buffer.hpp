#ifndef LEXER_BUFFER_HPP_
#define LEXER_BUFFER_HPP_

namespace gla {

namespace implementation_details {

template <class CharT>
class Lexer_buffer_iterator {
	public:
		using char_type         = CharT;

		Lexer_buffer_iterator (char_type *p) : m_p (p)
		{ }

		char_type *getraw ()
		{
			return m_p;
		}

		Lexer_buffer_iterator &operator++ ()
		{
			this->m_p = (char_type *)
			            (((uint64_t) (this->m_p + 1) & 0x0FFF) |
                        ((uint64_t) this->m_p & 0xFFFFFFFFFFFFF000));
			
			return *this;
		}

		char_type operator* ()
		{
			return *this->m_p;
		}

		bool operator== (Lexer_buffer_iterator &rhs)
		{
			return this->m_p == rhs.m_p;
		}

		bool operator!= (Lexer_buffer_iterator &rhs)
		{
			return this->m_p != rhs.m_p;
		}
	private:
		char_type *m_p;
};

template <class IStreamT>
class Lexer_buffer {
	public:
		using istream_type = IStreamT;
		using char_type = typename istream_type::char_type;
		using iterator = Lexer_buffer_iterator <char_type>;

		Lexer_buffer (istream_type *in) : m_in (in)
		{
			m_data_end = static_cast <char_type *> (aligned_alloc (4096, 4096));

			m_curr = m_data_end;

			m_first_buff_end = m_data_end + m_chars_in_buffer;
			m_second_buff_end = m_first_buff_end + m_chars_in_buffer;
		}

		~Lexer_buffer ()
		{
			if (m_second_buff_end < m_first_buff_end)
				free (m_second_buff_end - m_chars_in_buffer);
			else
				free (m_first_buff_end - m_chars_in_buffer);
		}
		
		char_type getchar ()
		{
			move ();

			return *m_curr;
		}

		iterator getpos () const
		{
			return iterator ((char_type *) m_curr);
		}

		bool is_empty () const
		{
			return m_curr == m_data_end;
		}
	private:
		istream_type *m_in;

		char_type *m_first_buff_end;
		char_type *m_second_buff_end;

		char_type *m_curr;
		
		char_type *m_data_end;

		static constexpr uint64_t m_chars_in_buffer = 2048 / sizeof (char_type);

		void move ()
		{
			if (m_curr == m_data_end)
				fill_buffer ();
			else 
				++m_curr;
		}

		void fill_buffer ()
		{
			if (m_data_end == m_first_buff_end)
				swap_buffers ();

			uint64_t n_chars = (m_first_buff_end - m_data_end) /
                               sizeof (char_type);

			n_chars = m_in->readsome (m_data_end, n_chars);

			m_data_end += n_chars;
		}

		void swap_buffers ()
		{
			char_type *tmp = m_first_buff_end;
			m_first_buff_end = m_second_buff_end;
			m_second_buff_end = tmp;

			m_data_end = m_first_buff_end - m_chars_in_buffer;
		}
};

} // end of implementation_details namespace

} // end of gla namespace

namespace std {

template <class CharT>
struct iterator_traits <gla::implementation_details::Lexer_buffer_iterator <CharT> > {
	using value_type = CharT;
	using iterator_category = std::input_iterator_tag;
};

}

#endif // LEXER_BUFFER_HPP_


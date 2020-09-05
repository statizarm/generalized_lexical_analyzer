#ifndef LEXER_BUFFER_HPP_
#define LEXER_BUFFER_HPP_

namespace gla {

namespace implementation_details {

template <class BufferT>
class Lexer_buffer_iterator {
	public:
		using char_type = typename BufferT::char_type;
		using buffer    = BufferT;

		Lexer_buffer_iterator (char_type *p) : m_p (p)
		{ }

		Lexer_buffer_iterator (const Lexer_buffer_iterator &other) = default;

		Lexer_buffer_iterator (Lexer_buffer_iterator &&rhs) = default;

		Lexer_buffer_iterator &
		operator= (const Lexer_buffer_iterator &rhs)
		{
			this->m_p = rhs.m_p;

			return *this;
		}

		char_type *getraw ()
		{
			return m_p;
		}

		Lexer_buffer_iterator &operator++ ()
		{
			if (*this->m_p == EOF) {
				this->m_p -= buffer::s_mem_volume;
			}

			++this->m_p;
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
		static constexpr uint64_t s_chars_in_buffer = 128;
		static constexpr uint64_t s_mem_volume      = s_chars_in_buffer * 2;

		using istream_type = IStreamT;
		using char_type = typename istream_type::char_type;
		using iterator  = Lexer_buffer_iterator <Lexer_buffer>;

		Lexer_buffer (istream_type *in) : m_in (in)
		{
			m_first_buff_end = m_mem + s_chars_in_buffer;
			m_second_buff_end = m_first_buff_end + s_chars_in_buffer;

			*(m_first_buff_end - 1)  = EOF;
			*(m_second_buff_end - 1) = EOF;

			m_curr = m_mem;
			*m_curr = EOF;
		}

		~Lexer_buffer ()
		{
		}

		char_type getchar ()
		{
			move ();

			return *m_curr;
		}
		
		char_type peek ()
		{
			if (is_empty ()) {
				fill_buffer ();
			}
			
			return *m_curr;
		}
			

		iterator getpos () const
		{
			return iterator (m_curr);
		}

		template <class It = iterator>
		Lexer_buffer &setpos (It &&pos) 
		{
			m_curr = pos.getraw ();

			return *this;
		}
	private:

		istream_type *m_in;

		char_type *m_first_buff_end;
		char_type *m_second_buff_end;

		char_type *m_curr;

		char_type m_mem[s_mem_volume];

		bool is_empty () const
		{
			return *m_curr == EOF;
		}

		void move ()
		{
			if (is_empty ()) {
				fill_buffer ();
			} else {
				++m_curr;
			}
		}

		void fill_buffer ()
		{
			if (m_curr == m_first_buff_end - 1) {
				//std::wcout << "che2" << std::endl;
				swap_buffers ();
			}

			uint64_t n_chars = (m_first_buff_end - m_curr - 1);
			//std::cout << "expected chars: " << n_chars << std::endl;

			n_chars = m_in->read (m_curr, n_chars).gcount ();

			*(m_curr + n_chars) = EOF;

			//std::cout << "recieved chars: " << n_chars << std::endl;

		}

		void swap_buffers ()
		{
			char_type *tmp = m_first_buff_end;
			m_first_buff_end = m_second_buff_end;
			m_second_buff_end = tmp;

			if (m_first_buff_end == m_mem + s_chars_in_buffer) {
				m_curr = m_mem;
			}
			//std::cout << "swap buffers" << std::endl;
		}
};

} // end of implementation_details namespace

} // end of gla namespace

namespace std {

template <class BufferT>
struct iterator_traits <gla::implementation_details
  ::Lexer_buffer_iterator <BufferT> > {
	using value_type = typename BufferT::char_type;
	using iterator_category = std::input_iterator_tag;
};

} // end of std namespace

#endif // LEXER_BUFFER_HPP_


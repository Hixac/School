#pragma once

namespace My {

	class Counter {
	public:
		inline static Counter& Get() { static Counter c; return c; }

		inline void IncComp() { m_compares++; }
		inline void IncMov() { m_moves++; }

		inline void AddComp(unsigned int num) { m_compares += num; }
		inline void AddMov(unsigned int num) { m_moves += num; }

		inline void SetBlockSize(unsigned int num) { m_block_size = num; }
		inline unsigned int GetBlockSize() { return m_block_size; }
		
		inline unsigned int GetComp() { return m_compares; }
		inline unsigned int GetMov() { return m_moves; }

		inline void Reset() { m_compares = 0; m_moves = 0; }
		
	private:
		Counter() = default;
		~Counter() = default;

		unsigned int m_block_size;
		unsigned int m_compares, m_moves;
	};
	
}

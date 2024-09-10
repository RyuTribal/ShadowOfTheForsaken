#pragma once

namespace SOF {
	template<typename BufferData>
	class Thread {
	public:
		Thread(std::string_view thread_name) : m_ThreadName(std::string(thread_name)){}
		~Thread(){}

		void Run(BufferData data) {

		}

		void WaitToFinish() {

		}

		void SwapBuffers() {

		}


	private:
		std::string m_ThreadName = "A Thread";
		BufferData m_Buffer1, m_Buffer2;
		BufferData* m_ReadPtr, m_WritePtr;
		std::condition_variable m_Conditional;

	};
}
#pragma once

#include <cassert>
#include <vector>
#include "../vector_erase_move_lastelement/vector_swap_erase.h"

namespace doom
{
	/// <summary>
	/// Why I made ComponentIterater
	/// 
	/// Think if you wanna Iterate over specific component of spawned entity
	/// You should do like this
	/// for(auto& entity : GetAllSpawnedEntity)
	/// {
	///		Renderer* renderer = entity.GetComponent<Renderer>();
	///		renderer->Draw();
	/// }
	/// 
	/// So Slow!!!!!!!!!!!!!
	/// 
	/// So We Do like this
	/// for(auto& renderer : RendererStaticInterator)
	/// {
	///		renderer->Draw();
	/// }
	/// 
	/// This is super fast
	/// Why this is fast?
	/// 1. This way don't use GetComponent, Call is decreased
	/// 
	/// 2. StaticIterator's elements is stored in vector, Vector is random access container.
	///	   This will make iterating much faster ( random access, Cache hit!!!! )
	/// 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class StaticContainer //Never inherit Component
	{
		using this_type = typename StaticContainer<T>;
	
	private:

		static inline std::vector<T*> mElements{};

		/// <summary>
		/// why don't use iterator
		/// iterator can be invalidated when vector's size is over capacity
		/// </summary>
		size_t mComponentStaticIndex;

		constexpr void AddToStaticContainer()
		{
			this_type::mElements.push_back(reinterpret_cast<T*>(this));
			this->mComponentStaticIndex = this_type::mElements.size() - 1;
		}

		void RemoveFromStaticContainer()
		{
			typename std::vector<T*>::iterator swapedElementIter = std::vector_swap_erase(this_type::mElements, this->mComponentStaticIndex);
			if (swapedElementIter != this_type::mElements.end())
			{
				//TODO : Need Test
				static_cast<this_type*>((*swapedElementIter))->mComponentStaticIndex = this->mComponentStaticIndex;
			}
		}

	protected:
		
		
		size_t GetStaticElementIndex() const
		{
			return this->mComponentStaticIndex;
		}

		static size_t GetStaticElementCount()
		{
			return this_type::mElements.size();
		}

		constexpr StaticContainer()
		{
			this->AddToStaticContainer();
		}

		virtual ~StaticContainer()
		{
			this->RemoveFromStaticContainer();
		}

	public:

		
		[[nodiscard]] static constexpr std::pair<T**, size_t> GetAllStaticComponents()
		{
			return std::make_pair(this_type::mElements.data(), this_type::mElements.size());
		}

		[[nodiscard]] static constexpr T* GetComponentWithIndex(unsigned int index)
		{
			assert(index >= 0);
			if (index < this_type::mElements.size())
			{
				return this_type::mElements[index];
			}
			else
			{
				return nullptr;
			}
		}

		/// <summary>
		/// return foremost component except for passed argumnet component
		/// </summary>
		/// <param name="excludedObject"></param>
		/// <returns></returns>
		[[nodiscard]] static constexpr T* GetForemostComponentWithHint(T* excludedObject)
		{
			for (unsigned int i = 0; i < this_type::mElements.size(); i++)
			{
				if (this_type::mElements[i] != excludedObject)
				{
					return this_type::mElements[i];
				}
			}
			
			return nullptr;
		}
	
	};

}
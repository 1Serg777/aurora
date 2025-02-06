#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include <string_view>
#include <stack>
#include <type_traits>
#include <vector>

namespace aurora
{
#define THIS_FILE __FILE__
#define THIS_FUNCTION __FUNCTION__
#define THIS_LINE __LINE__

#define NODISCARD [[nodiscard]]
#define NOEXCEPT noexcept

#define CLASS_DEFAULT_COPY(class_name)\
	class_name(const class_name& copy) = default;\
	class_name& operator=(const class_name& copy) = default

#define CLASS_DEFAULT_MOVE(class_name)\
	class_name(class_name&& move) = default;\
	class_name& operator=(class_name&& move) = default

#define CLASS_NO_COPY(class_name)\
	class_name(const class_name& copy) = delete;\
	class_name& operator=(const class_name& copy) = delete

#define CLASS_NO_MOVE(class_name)\
	class_name(class_name&& move) = delete;\
	class_name& operator=(class_name&& move) = delete

#define STRUCT_DEFAULT_COPY(struct_name) CLASS_DEFAULT_COPY(struct_name)

#define STRUCT_DEFAULT_MOVE(struct_name) CLASS_DEFAULT_MOVE(struct_name)

#define STRUCT_NO_COPY(struct_name) CLASS_NO_COPY(struct_name)

#define STRUCT_NO_MOVE(struct_name) CLASS_NO_MOVE(struct_name)

	template <typename T>
	std::string GenerateUniqueName(std::string_view name)
	{
		static int creationIdx{ 0 };

		std::string uniqueName{ name };
		uniqueName.append("_");
		uniqueName.append(std::to_string(creationIdx++));
		return uniqueName;
	}

	template <
		typename IdType,
		std::enable_if_t<std::is_unsigned_v<IdType>, bool> = true>
	class SeqIdGenerator
	{
	public:

		SeqIdGenerator() = default;
		~SeqIdGenerator() = default;

		CLASS_NO_COPY(SeqIdGenerator);
		CLASS_DEFAULT_MOVE(SeqIdGenerator);

		IdType GenerateUniqueId()
		{
			if (!freeIds.empty())
			{
				IdType id = freeIds.top();
				freeIds.pop();
				return id;
			}
			return idCounter++;
		}
		void FreeUniqueId(IdType id)
		{
			assert(IdValid(id) && "Invalid ID provided!");
			freeIds.push(id);
		}

		void Reset()
		{
			idCounter = 0;
			freeIds = std::stack<IdType>();
		}

		bool IdValid(IdType id) const
		{
			return (id >= 0) && (id < idCounter);
		}

	private:

		IdType idCounter{ 0 };
		std::stack<IdType> freeIds;
	};

	template<typename T>
	void IdleDeleter(T* ptr)
	{
		// DO NOTHING
	}

	// class CallbackStorage

	template <typename CallbackId, typename Callable>
	class CallbackStorage2
	{
	public:

		struct Callback
		{
			Callable callable{};
			CallbackId id{ 0 };
		};

		CallbackId AddCallback(Callable callable)
		{
			CallbackId uniqueCallbackId = idGen.GenerateUniqueId();

			Callback callback{};
			callback.id = uniqueCallbackId;
			callback.callable = callable;

			callbacks.push_back(std::move(callback));

			return uniqueCallbackId;
		}
		void RemoveCallback(CallbackId id)
		{
			auto pred = [id](const Callback& callback) {
				return callback.id == id;
				};

			auto begin_del_iter = std::remove_if(callbacks.begin(), callbacks.end(), pred);

			callbacks.erase(begin_del_iter, callbacks.end());

			idGen.FreeUniqueId(id);
		}

		template <class... Args>
		void Invoke(Args&&... args) const
		{
			for (const Callback& callback : callbacks)
			{
				callback.callable(std::forward<Args>(args)...);
			}
		}

	private:

		SeqIdGenerator<CallbackId> idGen;
		std::vector<Callback> callbacks;
	};
}
#pragma once
template <class value_type>
class value_lock
{
private:
	value_type* value;
	value_type unlock_value;

	bool locked = false;

public:
	value_lock(value_type* value, value_type lock_value, value_type unlock_value)
	{
		this->value = value;
		this->unlock_value = unlock_value;

		*value = lock_value;
		locked = true;
	}

	~value_lock()
	{
		if (locked)
		{
			*value = unlock_value;
			locked = false;
		}
	}

	void Unlock()
	{
		if (locked)
		{
			*value = unlock_value;
			locked = false;
		}
	}
};
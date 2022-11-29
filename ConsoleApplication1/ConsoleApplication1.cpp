#include<vector>
#include<iostream>
#include<string>
#include<algorithm>
#define INF 0x3f3f3f3f
using namespace std;

class BigInteger
{
public:
	using ll = long long;

	BigInteger() {};
	BigInteger(const string& s);
	BigInteger(ll a) :BigInteger(to_string(a)) {}
	BigInteger(const BigInteger& bInt) :nums(bInt.nums), isPositive(bInt.isPositive), length(bInt.length) {}
	BigInteger(BigInteger&& bInt) noexcept :nums(move(bInt.nums)), isPositive(bInt.isPositive), length(bInt.length) {}
	BigInteger(const vector<int>& vec, bool sign = true) :nums(vec), isPositive(sign) { cutLeadZero(); }

	friend istream& operator >>(istream& is, BigInteger& bInt)
	{
		string s;
		is >> s;
		bInt = move(BigInteger(s));
		return is;
	}
	friend ostream& operator <<(ostream& os, const BigInteger& bInt);
	operator string() const;
	const BigInteger& operator +() const { return *this; }
	BigInteger operator -() const
	{
		BigInteger tmp(*this);
		if (!tmp.isZero())
			tmp.isPositive = !isPositive;
		return tmp;
	}
	bool operator <(const BigInteger& bInt) const;
	bool operator <=(const BigInteger& bInt) const;
	bool operator ==(const BigInteger& bInt) const;
	BigInteger operator +(const BigInteger& bInt) const;
	BigInteger operator -(const BigInteger& bInt) const;
	BigInteger operator *(const BigInteger& bInt) const;
	pair<BigInteger, BigInteger> operator /(const BigInteger& bInt) const;
	int operator[](int idx) const { return nums[idx]; }
	BigInteger& operator =(const BigInteger& bInt)
	{
		if (bInt == *this)
			return *this;
		nums = bInt.nums;
		isPositive = bInt.isPositive;
		length = bInt.length;
		return *this;
	}
	BigInteger& operator =(BigInteger&& bInt)noexcept
	{
		nums = move(bInt.nums);
		isPositive = bInt.isPositive;
		length = bInt.length;
		return *this;
	}

	size_t size() const { return nums.size(); }
	void cutLeadZero();
	bool isZero() const;
	BigInteger absValue() const
	{
		return move(BigInteger(nums));
	}
	// 构造10^n n<=0会返回1 Structure of 10 ^ n n < = 0 returns 1
	static BigInteger e(size_t n)
	{
		if (n <= 0)
			return move(BigInteger(vector<int>(1, 1)));
		int m = n / digit;
		n -= m * digit;
		vector<int> ans(m);
		string s = "1";
		s += move(string(n, '0'));
		ans.push_back(stoi(s));
		return move(BigInteger(ans));
	}

private:
	// 低位到高位 Low to high
	vector<int> nums;
	// 符号位 The sign bit
	bool isPositive = 1;
	// 位数 digits
	int length = 0;
	// nums中的每一个元素代表digit位数据 Each element in the nums represents digit bits of data
	static int digit;
	static int mod;
};

int BigInteger::digit = 8;        // 8位  eight position
int BigInteger::mod = 100000000; // 1e9 可能用mask会更好？1e9 may use mask will be better?

BigInteger::BigInteger(const string& s)
{
	int n = s.size(), minIdx = 0;
	if (s[0] == '-')
		isPositive = false, minIdx = 1;
	else if (s[0] == '+')
		isPositive = true, minIdx = 1;
	for (int i = n - 1; i >= minIdx; i -= digit)
	{
		int beg = max(minIdx, i - digit + 1);
		nums.push_back(stoi(s.substr(beg, i - beg + 1)));
	}
	cutLeadZero();
}

ostream& operator <<(ostream& os, const BigInteger& bInt)
{
	os << (string)bInt;
	return os;
}

BigInteger::operator string() const
{
	string ans;
	if (!isPositive)
		ans += "-";
	int n = nums.size();
	for (int i = n - 1; i >= 0; i--)
	{
		string s = to_string(nums[i]);
		if (i != n - 1)
			ans += string(digit - s.size(), '0');
		ans += s;
	}
	return ans;
}

bool BigInteger::operator<(const BigInteger& bInt) const
{
	if (isPositive && !bInt.isPositive)
		return false;
	if (!isPositive && bInt.isPositive)
		return true;
	bool flag = true;
	if (!isPositive)
		flag = false; // 都为负数 Have a negative
	if (length < bInt.length)
		return flag;
	else if (length > bInt.length)
		return !flag;
	int n = size();
	for (int i = n - 1; i >= 0; i--)
	{
		if (nums[i] < bInt[i])
			return flag;
		else if (nums[i] > bInt[i])
			return !flag;
	}
	return false;
}

bool BigInteger::operator<=(const BigInteger& bInt) const
{
	if (isPositive && !bInt.isPositive)
		return false;
	if (!isPositive && bInt.isPositive)
		return true;
	bool flag = true;
	if (!isPositive)
		flag = false; // 都为负数
	if (length < bInt.length)
		return flag;
	else if (length > bInt.length)
		return !flag;
	int n = size();
	for (int i = n - 1; i >= 0; i--)
	{
		if (nums[i] < bInt[i])
			return flag;
		else if (nums[i] > bInt[i])
			return !flag;
	}
	return true;
}

bool BigInteger::operator==(const BigInteger& bInt) const
{
	if (length != bInt.length)
		return false;
	int n = size();
	for (int i = 0; i < n; i++)
		if (nums[i] != bInt[i])
			return false;
	return true;
}

BigInteger BigInteger::operator+(const BigInteger& bInt) const
{
	if (!bInt.isPositive)
		return *this - (-bInt); // 加上负数 = 减去其绝对值
	if (!isPositive)
		return bInt - (-*this); // 负数+正数 = 整数-(-负数) A negative number + positive = integer - (negative)
	// 要么都正 要么都负 Either are or are negative
	vector<int> ans;
	int n = size(), m = bInt.size(), sum = 0, i = 0, j = 0;
	while (i < n || j < m || sum)
	{
		if (i < n)
			sum += nums[i++];
		if (j < m)
			sum += bInt[j++];
		ans.push_back(sum % mod);
		sum /= mod;
	}
	return move(BigInteger(ans, isPositive));
}

BigInteger BigInteger::operator-(const BigInteger& bInt) const
{
	if (!bInt.isPositive)
		return *this + (-bInt); // 减去负数 = 加上其绝对值
	if (!isPositive)
		return -((-*this) + bInt); // 负数-正数 = -(-负数 + 正数)
	if (*this < bInt)
		return -(bInt - *this);
	// 只计算大数减去小数
	vector<int> ans;
	int i = 0, j = 0, n = size(), m = bInt.size(), sum = 0;
	while (i < n || j < m || sum)
	{
		if (i < n)
			sum += nums[i++];
		if (j < m)
			sum -= bInt[j++];
		int flag = 0;
		if (sum < 0)
		{
			flag = -1;
			sum += mod;
		}
		ans.push_back(sum);
		sum = flag;
	}
	return move(BigInteger(ans));
}

BigInteger BigInteger::operator*(const BigInteger& bInt) const
{
	int n = size(), m = bInt.size();
	vector<int> ans(n + m);
	using ll = long long;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			ll tmp = ans[i + j] + nums[i] * 1ll * bInt[j];
			ans[i + j] = tmp % mod;
			ans[i + j + 1] += tmp / mod;
		}
	}
	return move(BigInteger(ans, isPositive == bInt.isPositive));
}

pair<BigInteger, BigInteger> BigInteger::operator/(const BigInteger& bInt) const
{
	BigInteger a = absValue();
	BigInteger b = bInt.absValue();
	if (b.isZero())
		return pair<BigInteger, BigInteger>(*this, move(b));
	if (a < b)
		return pair<BigInteger, BigInteger>(move(BigInteger(0)), *this);
	int len = a.length - b.length + 1;
	string ans;
	if (isPositive != bInt.isPositive)
		ans = "-";
	for (int i = 0; i < len; i++)
	{
		BigInteger tmp = e(len - i - 1) * b;
		int times = 0;
		while (tmp <= a)
		{
			a = a - tmp;
			++times;
		}
		ans += times + '0';
	}
	a.isPositive = isPositive;
	return pair<BigInteger, BigInteger>(move(BigInteger(ans)), move(a));
}

void BigInteger::cutLeadZero()
{
	while (nums.size() > 1 && nums.back() == 0)
		nums.pop_back();
	if (nums.empty())
		length = 0;
	else
	{
		length = (nums.size() - 1) * digit + to_string(nums.back()).size();
	}
}

bool BigInteger::isZero() const
{
	return nums.size() == 1 && nums.back() == 0;
}
BigInteger c[], k;
void f(BigInteger n)
{
	cout << n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n * n;
}
int main()
{
	BigInteger a;
	cin >> a;
	f(a);
	return 0;
}


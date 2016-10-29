#include <iostream>
#include <string>
#include <vector>

#include <stdexcept>
#include <type_traits>


// The following is evil template "black magic". Only because we write it instead
// of the C++ standards committee. This first section is effectively a C++11
// implementation of std::make_integer_sequence which is coming in the C++14 standard
// Inspired heavily by numerous answers on StackOverflow, but written with useful names

// This type exists solely to be a holder for the
// template arguments, which is a sequence of ints
template <unsigned...> struct NumberSequence { };

// Let's make a special template struct who only exists
// to construct a new NumberSequence type, given N
template <unsigned N, unsigned StartIndex, unsigned CurrentNumber, unsigned... TrailingNumbers>
struct GenerateNumberSequence
{
    // A bit of recursion, our bread and butter for variadic templates. Notice
    // that we pass 5 arguments, instead of the 4 defined above. Effectively this
    // extends the length of the template argument list by 1 every time this recursion
    // happens. So we'll need to stop it once we reach CurrentNumber == StartIndex or
    // we may go on forever until the compiler craps out due to recursion depth. Note
    // that we're also recursively redefining the 'type' to be the next level down in
    // the recursion, so that eventually this top level type is typdef'd to the bottom.
    typedef typename GenerateNumberSequence<N, StartIndex, CurrentNumber - 1, CurrentNumber, TrailingNumbers...>::type type;
};

// Now we partially specialize the template for the case where
// CurrentNumber == StartIndex, which we use to stop infinite recursion
template <unsigned N, unsigned StartIndex, unsigned... TrailingNumbers>
struct GenerateNumberSequence<N, StartIndex, StartIndex, TrailingNumbers...>
{
    // The final type def, a NumberSequence from StartIndex to N
    typedef NumberSequence<StartIndex, TrailingNumbers...> type;
};

// A helper template for generating 0...N
// Equivalent of std::make_integer_sequence
template <unsigned N>
struct GenerateFullNumberSequence
    : GenerateNumberSequence<N, 0, N>
{
};

// A helper for generating StartIndex...N
template <unsigned N, unsigned StartIndex>
struct GenerateNumberSubSequence
    : GenerateNumberSequence<N, StartIndex, N>
{
};


// Recursive comparison of each individual character in a string
// The last bit with std::enable_if uses SFINAE (Substitution Failure Is Not An Error)
// to rule this function out and switch to the base case for the recursion when the Index == Length
template <unsigned Length, unsigned Index, typename Left, typename Right, typename std::enable_if<Index != Length, bool>::type = 0>
constexpr bool CompareCharacters( const Left& lhs, const Right& rhs )
{
    return lhs[Index] == rhs[Index] ? CompareCharacters<Length, Index + 1>( lhs, rhs ) : false;
}

// Recursion base case. If you run past the last index of
template <unsigned Length, unsigned Index, typename Left, typename Right, typename std::enable_if<Index == Length, bool>::type = 0>
constexpr bool CompareCharacters( const Left& lhs, const Right& rhs )
{
    return true;
}


// Helper type traits to determine the length of either
// a string literal or a StringConstant (specialized below)
template <typename T>
struct length_of
{
};

template <std::size_t N>
struct length_of< const char(&)[N] >
{
    static const std::size_t value = N - 1;
};

template <std::size_t N>
struct length_of< char[N] >
{
    static const std::size_t value = N - 1;
};


// This small class is the heart of the constant string implementation.
// It has constructors for string literals and individual chars, as well
// as operators to interact with string literals or other instances. This
// allows for it to have a very natural interface, and it's all constexpr
// Inspired heavily by a class described in a presentation by Scott Schurr
// at Boostcon:
// https://github.com/boostcon/cppnow_presentations_2012/blob/master/wed/schurr_cpp11_tools_for_class_authors.pdf
template <std::size_t N>
class StringConstant
{
public:
    // The main constructor, useful for humans, although it is easier to
    // use the StringFactory function below to avoid the template prameter
    constexpr StringConstant( const char(&value)[N + 1] )
        : StringConstant( value, typename GenerateFullNumberSequence<N - 1>::type( ) )
    {
    }

    // Constructor which takes individual chars. Allows for unpacking
    // parameter packs directly into the constructor
    template <typename... Characters>
    constexpr StringConstant( Characters... characters )
        : m_value{ characters..., '\0' }
    {
    }

    // Addition operator, covers both string literals and other
    // instances of StringConstant thanks to auto and decltype
    template <typename T>
    constexpr auto operator+( const T& rhs ) const -> decltype( ConcatStrings( *this, rhs ) )
    {
        return ConcatStrings( *this, rhs );
    }
    
    // Equality operator, again, covers both string literals and other instances
    // The observant reader may notice there is no reason to use auto here, we clearly know
    // that operator== returns bool. However, without the trailing decltype, g++4.8 complains
    // that *this is not a constant expression. With it, it believes it is. Seems like a quirk.
    template <typename T, typename std::enable_if<length_of<T>::value == N, bool>::type = 0>
    constexpr auto operator==( const T& rhs ) const -> decltype( CompareCharacters<N, 0>( *this, rhs ) )
    {
        return CompareCharacters<N, 0>( *this, rhs );
    }
    
    // Different length strings can never be equal
    template <typename T, typename std::enable_if<length_of<T>::value != N, bool>::type = 0>
    constexpr bool operator==( const T& rhs ) const
    {
        return false;
    }

    // Array subscript operator, with some basic range checking
    constexpr char operator[]( std::size_t index ) const
    {
        return index < m_size ? m_value[index] : throw std::out_of_range("Index out of range");
    }

    constexpr const char* Get( ) const { return m_value; }
    constexpr std::size_t Size( ) const { return m_size; }

private:
    // Private constructor which mainly serves as a necessary layer of indirection
    template <unsigned... Indexes>
    constexpr StringConstant( const char(&value)[N + 1], NumberSequence<Indexes...> __attribute__((unused)) dummy )
        : StringConstant( value[Indexes]... )
    {
    }

    const char m_value[N + 1];
    const std::size_t m_size = N;
};

// Specialize the length_of trait for the StringConstant class
template <std::size_t N>
struct length_of< StringConstant<N> >
{
    static const std::size_t value = N;
};

template <std::size_t N>
struct length_of< const StringConstant<N>& >
{
    static const std::size_t value = N;
};


// A helper function for concatenating StringConstants

// Less than human friendly concat function, wrapped by a huamn friendly one below
template <typename Left, typename Right, unsigned... IndexesLeft, unsigned... IndexesRight>
constexpr StringConstant<sizeof...(IndexesLeft) + sizeof...(IndexesRight)> ConcatStrings( const Left& lhs, const Right& rhs, NumberSequence<IndexesLeft...> __attribute__((unused)) dummy1, NumberSequence<IndexesRight...> __attribute__((unused)) dummy2 )
{
    return StringConstant<sizeof...(IndexesLeft) + sizeof...(IndexesRight)>( lhs[IndexesLeft]..., rhs[IndexesRight]... );
}

// Human friendly concat function for string literals
template <typename Left, typename Right>
constexpr StringConstant<length_of<Left>::value + length_of<Right>::value> ConcatStrings( const Left& lhs, const Right& rhs )
{
    return ConcatStrings( lhs, rhs, typename GenerateFullNumberSequence<length_of<decltype(lhs)>::value - 1>::type( ), typename GenerateFullNumberSequence<length_of<decltype(rhs)>::value - 1>::type( ) );
}


// Finally, operators for dealing with a string literal LHS and StringConstant RHS

// Addition operator
template <std::size_t LengthLeft, std::size_t LengthRight>
constexpr StringConstant<LengthLeft - 1 + LengthRight> operator+( const char(&lhs)[LengthLeft], const StringConstant<LengthRight>& rhs )
{
    return ConcatStrings( lhs, rhs );
}

// Equality operator
template <std::size_t N>
constexpr bool operator==( const char(&lhs)[N + 1], const StringConstant<N>& rhs )
{
    return CompareCharacters<N, 0>( lhs, rhs );
}

// Different length strings can never be equal
template <std::size_t X, std::size_t Y>
constexpr bool operator==( const char(&lhs)[X + 1], const StringConstant<Y>& rhs )
{
    return false;
}


// A helper factory function for creating FixedStringConstant objects
// which handles figuring out the length of the string for you
template <unsigned N>
constexpr StringConstant<N - 1> StringFactory( const char(&value)[N] )
{
    return StringConstant<N - 1>( value );
}


// Test code, tested with g++ 4.8 and clang++ 3.3
/*
int main( )
{
    constexpr auto foo = StringFactory( "foo" );
    constexpr auto bar = StringFactory( "bar" );

    constexpr auto foobar = foo + bar;

    // Test cases as static_assert statements, change
    // any of these if you'd like to see the compile fail

    // Equality, both directions
    static_assert( foobar == "foobar", "Failure, is unacceptable" );
    static_assert( "foobar" == foobar, "Failure, is unacceptable" );

    // On the fly concat, and equality, both directions
    static_assert( ( foo + "bar" ) == "foobar", "Failure, is unacceptable" );
    static_assert( "foobar" == ( "foo" + bar ), "Failure, is unacceptable" );

    // Odds and ends
    static_assert( (foo + bar)[3] == 'b', "Failure, is unacceptable" );
    static_assert( (foo + bar).Size( ) == 6, "Failure, is unacceptable" );
  
    std::cout << "Hello, world" << std::endl;

    return 0;
}
*/

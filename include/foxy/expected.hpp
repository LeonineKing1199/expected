#ifndef FOXY_EXPECTED_HPP_
#define FOXY_EXPECTED_HPP_

#include <type_traits>
#include <fit/infix.hpp>
#include <fit/function.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>

namespace foxy
{
  // forward declare so we can declare as a friend
  struct fmap;

  template <
    typename T, typename E,
    typename = std::enable_if_t<!std::is_same_v<T, E>>
  >
  struct expected
  {
  public:
    using value_type     = T;
    using exception_type = E;

  private:
    boost::variant<T, E> data_;

    friend fmap;

  public:
    // only enable the default constructor if
    // T and E are default constructible
    template <
      typename = std::enable_if_t<std::is_default_constructible_v<T>>
    >
    expected(void)
    : data_{}
    {}

    // only enable the copy constructor if
    // T and E are copy constructible
    template <
      typename = std::enable_if_t<std::is_copy_constructible_v<T>>,
      typename = std::enable_if_t<std::is_copy_constructible_v<E>>
    >
    expected(expected const& rhs)
    : data_{rhs.data_}
    {}

    // only enable the move constructor if
    // T and E are move constructible
    template <
      typename = std::enable_if_t<std::is_move_constructible_v<T>>,
      typename = std::enable_if_t<std::is_move_constructible_v<E>>
    >
    expected(expected&& rhs)
    : data_{std::move(rhs).data_}
    {}

    template <
      typename = std::enable_if_t<std::is_copy_constructible_v<T>>
    >
    expected(T const& t)
    : data_{t}
    {}

    template <
      typename = std::enable_if_t<std::is_move_constructible_v<T>>
    >
    expected(T&& t)
    : data_{std::move(t)}
    {}

    template <
      typename = std::enable_if_t<std::is_copy_constructible_v<E>>
    >
    expected(E const& e)
    : data_{e}
    {}

    template <
      typename = std::enable_if_t<std::is_move_constructible_v<E>>
    >
    expected(E&& e)
    : data_{std::move(e)}
    {}

    auto is_valid(void) const noexcept -> bool
    {
      return data_.which() == 0;
    }

    // (>>=) :: Monad m => m a -> (a -> m b) -> m b
    template <typename F>
    auto operator>>=(F&& f) const
    {
      using boost::get;

      using new_expected_type =
        decltype(std::forward<F>(f)(std::declval<T>()));

      using new_value_type = typename new_expected_type::value_type;

      using return_type = expected<new_value_type, E>;

      if (data_.which() == 1) { return return_type{get<E>(data_)}; }

      auto&& val = get<T>(data_);
      auto&& ret = std::forward<F>(f)(
        std::forward<decltype(val)>(val));

      return return_type{std::forward<decltype(ret)>(ret)};
    }

    // will actually throw if expected<T, E> is in
    // an invalid state
    auto get(void) const -> T
    {
      using boost::get;
      return get<T>(data_);
    }
  };

  // fmap :: Functor f => (a -> b) -> f a -> f b
  struct fmap
  {
    template <typename F, typename T, typename E>
    auto operator()(F&& f, expected<T, E> const& e) const
      -> expected<decltype(std::forward<F>(f)(std::declval<T>())), E>
    {
      using boost::get;

      try {
        if (e.data_.which() == 0) {
          return {std::forward<F>(f)(get<T>(e.data_))};
        }
        return {get<E>(e.data_)};

      } catch (E const& err) {
        return {err};
      }
    }
  };

  // (<$>) :: Functor f => (a -> b) -> f a -> f b
  FIT_STATIC_FUNCTION($) = fit::infix(fmap());
}

#endif // FOXY_EXPECTED_HPP_
#ifndef FOXY_EXPECTED_HPP_
#define FOXY_EXPECTED_HPP_

#include <type_traits>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>

namespace foxy
{
  struct fmap;

  template <
    typename T, typename E,
    typename = std::enable_if_t<!std::is_same_v<T, E>>
  >
  struct expected
  {
  private:
    boost::variant<T, E> data_;

    friend fmap;

  public:
    template <
      typename = std::enable_if_t<std::is_default_constructible_v<T>>,
      typename = std::enable_if_t<std::is_default_constructible_v<E>>
    >
    expected(void)
    : data_{}
    {}

    template <
      typename = std::enable_if_t<std::is_copy_constructible_v<T>>,
      typename = std::enable_if_t<std::is_copy_constructible_v<E>>
    >
    expected(expected const& rhs)
    : data_{rhs.data_}
    {}

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
  };

  struct fmap
  {
    template <typename F, typename T, typename E>
    auto operator()(F&& f, expected<T, E> const& e) const
      -> expected<decltype(std::forward<F>(f)(std::declval<T>())), E>
    {
      using boost::get;
      using result_type = expected<decltype(std::forward<F>(f)(std::declval<T>())), E>;

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
}

#endif // FOXY_EXPECTED_HPP_
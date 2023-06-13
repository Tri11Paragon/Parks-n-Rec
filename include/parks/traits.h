//
// Created by brett on 6/8/23.
//

#ifndef PARKSNREC_TRAITS_H
#define PARKSNREC_TRAITS_H

#include <type_traits>
#include <iostream>

namespace parks {
    template <typename T, typename Enable = void>
    struct is_streamable : public std::false_type {};

    template <typename T>
    struct is_streamable<T, std::enable_if_t<std::is_same<decltype(std::declval<std::ostream&>() << std::declval<T>()), std::ostream&>::value>> : public std::true_type {};
}

#endif //PARKSNREC_TRAITS_H

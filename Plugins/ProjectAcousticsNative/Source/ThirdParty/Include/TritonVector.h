// Copyright (c) 2022 Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

namespace Triton
{
#pragma pack(push, 4)
    template <typename T>
    union Vector3 {
        struct
        {
            T x;
            T y;
            T z;
        };
        T arr[3];

        static Vector3 Zero()
        {
            return {{0, 0, 0}};
        }
    };
#pragma pack(pop)

    using Vec3i = Vector3<int>;
    using Vec3f = Vector3<float>;
    using Vec3d = Vector3<double>;
    using Vec3u = Vector3<uint32_t>;

#pragma pack(push, 4)
    template <typename T>
    struct AABox {
        Vector3<T> MinCorner;
        Vector3<T> MaxCorner;
    };
#pragma pack(pop)
} // namespace Triton

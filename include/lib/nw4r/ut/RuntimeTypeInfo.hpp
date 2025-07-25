#pragma once

namespace nw4r {
    namespace ut {
        #define NW4R_UT_RUNTIME_TYPEINFO \
            virtual const nw4r::ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const { \
                return &typeInfo; \
            } \
            static  const nw4r::ut::detail::RuntimeTypeInfo typeInfo

        #define NW4R_UT_GET_RUNTIME_TYPEINFO(T) \
            const nw4r::ut::detail::RuntimeTypeInfo T::typeInfo(0);

        #define NW4R_UT_GET_DERIVED_RUNTIME_TYPEINFO(T, D) \
            const nw4r::ut::detail::RuntimeTypeInfo T::typeInfo(&D::typeInfo);

        namespace detail {
            struct RuntimeTypeInfo {
                explicit RuntimeTypeInfo(const RuntimeTypeInfo* base) : mParentTypeInfo(base) {}

                bool IsDerivedFrom(const RuntimeTypeInfo* base) const {
                    for (const RuntimeTypeInfo* it = this; it != 0;
                        it = it->mParentTypeInfo) {
                        if (it == base) {
                            return true;
                        }
                    }
                    return false;
                }
                const RuntimeTypeInfo* mParentTypeInfo; // 0x00
            };

            template<typename T> inline const RuntimeTypeInfo* GetTypeInfoFromPtr_(T* pPtr) {
                return &pPtr->typeInfo;
            }
        }

        template<typename TDerived, typename TBase> inline TDerived DynamicCast(TBase* pPtr) {
            const detail::RuntimeTypeInfo* pDerivedTypeInfo = detail::GetTypeInfoFromPtr_(static_cast<TDerived>(0));
            if (pPtr->GetRuntimeTypeInfo()->IsDerivedFrom(pDerivedTypeInfo)) {
                return static_cast<TDerived>(pPtr);
            }
            return NULL;
        }
    }
}


/*
 * THIS IS AN AUTO-GENERATED FILE. DO NOT MODIFY AS CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef VISITABLE_TYPE_TRAIT
#define VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct visitorSelector {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct visitorSelector<true> {
    template<typename T, class Visitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
        visitor.visit(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<typename T>
struct isVisitable {
    static const bool value = false;
};

template<typename T, class Visitor>
void doVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, Visitor &visitor) {
    visitorSelector<isVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, visitor);
}
#endif

#ifndef TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#define TRIPLET_FORWARD_VISITABLE_TYPE_TRAIT
#include <cstdint>
#include <string>
#include <utility>

template<bool b>
struct tripletForwardVisitorSelector {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)preVisit;
        (void)postVisit;
        std::forward<Visitor>(visit)(fieldIdentifier, std::move(typeName), std::move(name), value);
    }
};

template<>
struct tripletForwardVisitorSelector<true> {
    template<typename T, class PreVisitor, class Visitor, class PostVisitor>
    static void impl(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
        (void)fieldIdentifier;
        (void)typeName;
        (void)name;
        // Apply preVisit, visit, and postVisit on value.
        value.accept(preVisit, visit, postVisit);
    }
};

template<typename T>
struct isTripletForwardVisitable {
    static const bool value = false;
};

template< typename T, class PreVisitor, class Visitor, class PostVisitor>
void doTripletForwardVisit(uint32_t fieldIdentifier, std::string &&typeName, std::string &&name, T &value, PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
    tripletForwardVisitorSelector<isTripletForwardVisitable<T>::value >::impl(fieldIdentifier, std::move(typeName), std::move(name), value, std::move(preVisit), std::move(visit), std::move(postVisit)); // NOLINT
}
#endif


#ifndef CALVALUES_HPP
#define CALVALUES_HPP

#ifdef WIN32
    // Export symbols if compile flags "LIB_SHARED" and "LIB_EXPORTS" are set on Windows.
    #ifdef LIB_SHARED
        #ifdef LIB_EXPORTS
            #define LIB_API __declspec(dllexport)
        #else
            #define LIB_API __declspec(dllimport)
        #endif
    #else
        // Disable definition if linking statically.
        #define LIB_API
    #endif
#else
    // Disable definition for non-Win32 systems.
    #define LIB_API
#endif

#include <string>
#include <utility>

using namespace std::string_literals; // NOLINT
class LIB_API calValues {
    private:
        static constexpr const char* TheShortName = "calValues";
        static constexpr const char* TheLongName = "calValues";

    public:
        inline static int32_t ID() {
            return 2001;
        }
        inline static const std::string ShortName() {
            return TheShortName;
        }
        inline static const std::string LongName() {
            return TheLongName;
        }

    public:
        calValues() = default;
        calValues(const calValues&) = default;
        calValues& operator=(const calValues&) = default;
        calValues(calValues&&) = default;
        calValues& operator=(calValues&&) = default;
        ~calValues() = default;

    public:
        inline calValues& cameraId(const int &v) noexcept {
            m_cameraId = v;
            return *this;
        }
        inline int cameraId() const noexcept {
            return m_cameraId;
        }
        inline calValues& m00(const double &v) noexcept {
            m_m00 = v;
            return *this;
        }
        inline double m00() const noexcept {
            return m_m00;
        }
        inline calValues& m01(const double &v) noexcept {
            m_m01 = v;
            return *this;
        }
        inline double m01() const noexcept {
            return m_m01;
        }
        inline calValues& m02(const double &v) noexcept {
            m_m02 = v;
            return *this;
        }
        inline double m02() const noexcept {
            return m_m02;
        }
        inline calValues& m10(const double &v) noexcept {
            m_m10 = v;
            return *this;
        }
        inline double m10() const noexcept {
            return m_m10;
        }
        inline calValues& m11(const double &v) noexcept {
            m_m11 = v;
            return *this;
        }
        inline double m11() const noexcept {
            return m_m11;
        }
        inline calValues& m12(const double &v) noexcept {
            m_m12 = v;
            return *this;
        }
        inline double m12() const noexcept {
            return m_m12;
        }
        inline calValues& m20(const double &v) noexcept {
            m_m20 = v;
            return *this;
        }
        inline double m20() const noexcept {
            return m_m20;
        }
        inline calValues& m21(const double &v) noexcept {
            m_m21 = v;
            return *this;
        }
        inline double m21() const noexcept {
            return m_m21;
        }
        inline calValues& m22(const double &v) noexcept {
            m_m22 = v;
            return *this;
        }
        inline double m22() const noexcept {
            return m_m22;
        }
        inline calValues& d0(const double &v) noexcept {
            m_d0 = v;
            return *this;
        }
        inline double d0() const noexcept {
            return m_d0;
        }
        inline calValues& d1(const double &v) noexcept {
            m_d1 = v;
            return *this;
        }
        inline double d1() const noexcept {
            return m_d1;
        }
        inline calValues& d2(const double &v) noexcept {
            m_d2 = v;
            return *this;
        }
        inline double d2() const noexcept {
            return m_d2;
        }
        inline calValues& d3(const double &v) noexcept {
            m_d3 = v;
            return *this;
        }
        inline double d3() const noexcept {
            return m_d3;
        }
        inline calValues& d4(const double &v) noexcept {
            m_d4 = v;
            return *this;
        }
        inline double d4() const noexcept {
            return m_d4;
        }

    public:
        template<class Visitor>
        inline void accept(uint32_t fieldId, Visitor &visitor) {
            (void)fieldId;
            (void)visitor;
//            visitor.preVisit(ID(), ShortName(), LongName());
            if (1 == fieldId) {
                doVisit(1, std::move("int"s), std::move("cameraId"s), m_cameraId, visitor);
                return;
            }
            if (2 == fieldId) {
                doVisit(2, std::move("double"s), std::move("m00"s), m_m00, visitor);
                return;
            }
            if (3 == fieldId) {
                doVisit(3, std::move("double"s), std::move("m01"s), m_m01, visitor);
                return;
            }
            if (4 == fieldId) {
                doVisit(4, std::move("double"s), std::move("m02"s), m_m02, visitor);
                return;
            }
            if (5 == fieldId) {
                doVisit(5, std::move("double"s), std::move("m10"s), m_m10, visitor);
                return;
            }
            if (6 == fieldId) {
                doVisit(6, std::move("double"s), std::move("m11"s), m_m11, visitor);
                return;
            }
            if (7 == fieldId) {
                doVisit(7, std::move("double"s), std::move("m12"s), m_m12, visitor);
                return;
            }
            if (8 == fieldId) {
                doVisit(8, std::move("double"s), std::move("m20"s), m_m20, visitor);
                return;
            }
            if (9 == fieldId) {
                doVisit(9, std::move("double"s), std::move("m21"s), m_m21, visitor);
                return;
            }
            if (10 == fieldId) {
                doVisit(10, std::move("double"s), std::move("m22"s), m_m22, visitor);
                return;
            }
            if (11 == fieldId) {
                doVisit(11, std::move("double"s), std::move("d0"s), m_d0, visitor);
                return;
            }
            if (12 == fieldId) {
                doVisit(12, std::move("double"s), std::move("d1"s), m_d1, visitor);
                return;
            }
            if (13 == fieldId) {
                doVisit(13, std::move("double"s), std::move("d2"s), m_d2, visitor);
                return;
            }
            if (14 == fieldId) {
                doVisit(14, std::move("double"s), std::move("d3"s), m_d3, visitor);
                return;
            }
            if (15 == fieldId) {
                doVisit(15, std::move("double"s), std::move("d4"s), m_d4, visitor);
                return;
            }
//            visitor.postVisit();
        }

        template<class Visitor>
        inline void accept(Visitor &visitor) {
            visitor.preVisit(ID(), ShortName(), LongName());
            doVisit(1, std::move("int"s), std::move("cameraId"s), m_cameraId, visitor);
            doVisit(2, std::move("double"s), std::move("m00"s), m_m00, visitor);
            doVisit(3, std::move("double"s), std::move("m01"s), m_m01, visitor);
            doVisit(4, std::move("double"s), std::move("m02"s), m_m02, visitor);
            doVisit(5, std::move("double"s), std::move("m10"s), m_m10, visitor);
            doVisit(6, std::move("double"s), std::move("m11"s), m_m11, visitor);
            doVisit(7, std::move("double"s), std::move("m12"s), m_m12, visitor);
            doVisit(8, std::move("double"s), std::move("m20"s), m_m20, visitor);
            doVisit(9, std::move("double"s), std::move("m21"s), m_m21, visitor);
            doVisit(10, std::move("double"s), std::move("m22"s), m_m22, visitor);
            doVisit(11, std::move("double"s), std::move("d0"s), m_d0, visitor);
            doVisit(12, std::move("double"s), std::move("d1"s), m_d1, visitor);
            doVisit(13, std::move("double"s), std::move("d2"s), m_d2, visitor);
            doVisit(14, std::move("double"s), std::move("d3"s), m_d3, visitor);
            doVisit(15, std::move("double"s), std::move("d4"s), m_d4, visitor);
            visitor.postVisit();
        }

        template<class PreVisitor, class Visitor, class PostVisitor>
        inline void accept(PreVisitor &&preVisit, Visitor &&visit, PostVisitor &&postVisit) {
            (void)visit; // Prevent warnings from empty messages.
            std::forward<PreVisitor>(preVisit)(ID(), ShortName(), LongName());
            doTripletForwardVisit(1, std::move("int"s), std::move("cameraId"s), m_cameraId, preVisit, visit, postVisit);
            doTripletForwardVisit(2, std::move("double"s), std::move("m00"s), m_m00, preVisit, visit, postVisit);
            doTripletForwardVisit(3, std::move("double"s), std::move("m01"s), m_m01, preVisit, visit, postVisit);
            doTripletForwardVisit(4, std::move("double"s), std::move("m02"s), m_m02, preVisit, visit, postVisit);
            doTripletForwardVisit(5, std::move("double"s), std::move("m10"s), m_m10, preVisit, visit, postVisit);
            doTripletForwardVisit(6, std::move("double"s), std::move("m11"s), m_m11, preVisit, visit, postVisit);
            doTripletForwardVisit(7, std::move("double"s), std::move("m12"s), m_m12, preVisit, visit, postVisit);
            doTripletForwardVisit(8, std::move("double"s), std::move("m20"s), m_m20, preVisit, visit, postVisit);
            doTripletForwardVisit(9, std::move("double"s), std::move("m21"s), m_m21, preVisit, visit, postVisit);
            doTripletForwardVisit(10, std::move("double"s), std::move("m22"s), m_m22, preVisit, visit, postVisit);
            doTripletForwardVisit(11, std::move("double"s), std::move("d0"s), m_d0, preVisit, visit, postVisit);
            doTripletForwardVisit(12, std::move("double"s), std::move("d1"s), m_d1, preVisit, visit, postVisit);
            doTripletForwardVisit(13, std::move("double"s), std::move("d2"s), m_d2, preVisit, visit, postVisit);
            doTripletForwardVisit(14, std::move("double"s), std::move("d3"s), m_d3, preVisit, visit, postVisit);
            doTripletForwardVisit(15, std::move("double"s), std::move("d4"s), m_d4, preVisit, visit, postVisit);
            std::forward<PostVisitor>(postVisit)();
        }

    private:
        int m_cameraId{  }; // field identifier = 1.
        double m_m00{ 0.0 }; // field identifier = 2.
        double m_m01{ 0.0 }; // field identifier = 3.
        double m_m02{ 0.0 }; // field identifier = 4.
        double m_m10{ 0.0 }; // field identifier = 5.
        double m_m11{ 0.0 }; // field identifier = 6.
        double m_m12{ 0.0 }; // field identifier = 7.
        double m_m20{ 0.0 }; // field identifier = 8.
        double m_m21{ 0.0 }; // field identifier = 9.
        double m_m22{ 0.0 }; // field identifier = 10.
        double m_d0{ 0.0 }; // field identifier = 11.
        double m_d1{ 0.0 }; // field identifier = 12.
        double m_d2{ 0.0 }; // field identifier = 13.
        double m_d3{ 0.0 }; // field identifier = 14.
        double m_d4{ 0.0 }; // field identifier = 15.
};


template<>
struct isVisitable<calValues> {
    static const bool value = true;
};
template<>
struct isTripletForwardVisitable<calValues> {
    static const bool value = true;
};
#endif


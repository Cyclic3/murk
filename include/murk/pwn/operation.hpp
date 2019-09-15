//#pragma once

//#include "murk/pwn/capstone_iface.hpp"
//#include "murk/data.hpp"

//#include "cppthings/movable_ptr.hpp"

//#include <variant>
//#include <vector>

//namespace murk::pwn {
//  namespace location {
//    struct reg {
//      std::string name;

//      inline bool operator==(const reg& other) const {
//        return name == other.name;
//      }
//      inline bool operator!=(const reg& other) const {
//        return name != other.name;
//      }
//    };
//    struct address {
//      uint64_t loc;

//      inline bool operator==(const address& other) const {
//        return loc == other.loc;
//      }
//      inline bool operator!=(const address& other) const {
//        return loc != other.loc;
//      }
//    };
////    struct stack {
////      inline bool operator==(const address&) const { return true; }
////      inline bool operator!=(const address&) const { return false; }
////    };
//    struct literal {
//      uint64_t val;

//      inline bool operator==(const literal& other) const {
//        return val == other.val;
//      }
//      inline bool operator!=(const literal& other) const {
//        return val != other.val;
//      }
//    };
//  }

//  using location_t = std::variant<
//    location::reg,
//    location::address,
////    location::stack,
//    location::literal
//  >;

//  namespace effect {
//    struct binary_op {
//      enum class type_t {
//        mov,
//        add,
//        sub,
//        mul,
//        div
//      };

//      type_t type;
//      location_t from;
//      location_t to;

//      inline bool operator==(const binary_op& other) const {
//        return type == other.type && from == other.from && to == other.to;
//      }
//      inline bool operator!=(const binary_op& other) const {
//        return type != other.type || from != other.from || to != other.to;
//      }
//    };
//    struct push {
//      location_t from;

//      inline bool operator==(const push& other) const {
//        return from == other.from;
//      }
//      inline bool operator!=(const push& other) const {
//        return from != other.from;
//      }
//    };
//    struct pop {
//      location_t to;

//      inline bool operator==(const pop& other) const {
//        return to == other.to;
//      }
//      inline bool operator!=(const pop& other) const {
//        return to != other.to;
//      }
//    };
//    struct interrupt {
//      uint64_t id;

//      inline bool operator==(const interrupt& other) const {
//        return id == other.id;
//      }
//      inline bool operator!=(const interrupt& other) const {
//        return id != other.id;
//      }
//    };
//    struct syscall {
//      inline bool operator==(const syscall&) const { return true; }
//      inline bool operator!=(const syscall&) const { return false; }
//    };
//  }

//  using effect_t = std::variant<
//    effect::binary_op,
//    effect::push,
//    effect::pop,
//    effect::interrupt,
//    effect::syscall
//  >;

//  // Describes the effects of an instruction
//  using operation_t = std::vector<effect_t>;
//}

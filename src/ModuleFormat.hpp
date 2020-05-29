//
// Created by chorm on 2020-05-28.
//

#ifndef ORKA_FRAMEWORK_MODULEFORMAT_HPP
#define ORKA_FRAMEWORK_MODULEFORMAT_HPP

#include <lclib-c++/BinaryIOHelpers.hpp>
#include <lclib-c++/Version.hpp>

namespace orka::framework{
    constexpr lclib::io::MagicNumbers<uint8_t[4]> MOD_MAGIC{0xEE,0x4F,0x4B,0x4D};

    enum ConstantTag:uint8_t{
        Const_Utf8,
        Const_i32,
        Const_i64,
        Const_u32,
        Const_u64,
        Const_Type,
        Const_Version
    };
    template<ConstantTag> struct ConstantEntry{
        uint16_t item;
        friend lclib::io::DataInputStream& operator>>(lclib::io::DataInputStream& in,ConstantEntry& entry){
            return in >> entry.item;
        }
        friend lclib::io::DataOutputStream& operator<<(lclib::io::DataOutputStream& out,const ConstantEntry& entry){
            return out << entry.item;
        }
    };
    struct Constant{
        std::variant<std::string,std::int32_t,std::int64_t,std::uint32_t,std::uint64_t,ConstantEntry<ConstantTag::Const_Utf8>,ConstantEntry<ConstantTag::Const_Utf8>>
            value;
        friend lclib::io::DataInputStream& operator>>(lclib::io::DataInputStream&,Constant&);
        friend lclib::io::DataOutputStream& operator<<(lclib::io::DataOutputStream&,const Constant&);
    };
    template<ConstantTag tag> decltype(auto) get_entry(ConstantEntry<tag> entry,const std::vector<Constant>& pool){
        return std::get<static_cast<std::size_t>(tag)>(pool.at(entry.item));
    }
    enum class DependencyOrder:uint8_t{
        Required,
        OptionalAfter,
        OptionalBefore,
        OptionalNone
    };
    struct Dependency{
        ConstantEntry<ConstantTag::Const_Utf8> modName;
        ConstantEntry<ConstantTag::Const_Version> modVersion;
        DependencyOrder order;

        friend lclib::io::DataInputStream& operator>>(lclib::io::DataInputStream&,Dependency&);
        friend lclib::io::DataOutputStream& operator<<(lclib::io::DataOutputStream&,const Dependency&);
    };

    struct ModuleFile{
        lclib::io::MagicNumbers<uint8_t[4]> magic{MOD_MAGIC};
        lclib::version::Version ver;
        lclib::io::variable_sized<std::vector<Constant>,uint16_t> constants;
        ConstantEntry<ConstantTag::Const_Utf8> mod_name;
        ConstantEntry<ConstantTag::Const_Version> mod_version;
        lclib::io::variable_sized<std::vector<Dependency>,uint16_t> dependencies;

        friend lclib::io::DataInputStream& operator>>(lclib::io::DataInputStream&,ModuleFile&);
        friend lclib::io::DataOutputStream& operator<<(lclib::io::DataOutputStream&,const ModuleFile&);
    };
}

#endif //ORKA_FRAMEWORK_MODULEFORMAT_HPP

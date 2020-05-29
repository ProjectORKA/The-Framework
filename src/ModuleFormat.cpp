//
// Created by chorm on 2020-05-28.
//

#include "ModuleFormat.hpp"

lclib::io::DataOutputStream & orka::framework::operator<<(lclib::io::DataOutputStream & out, const orka::framework::Constant & entry) {
    return out << static_cast<ConstantTag>(entry.value.index()) << entry.value;
}

lclib::io::DataInputStream &orka::framework::operator>>(lclib::io::DataInputStream & in, orka::framework::Constant& item) {
    auto tag{in.read<ConstantTag>()};
    switch(tag){
        case ConstantTag::Const_Utf8:
            item.value.emplace<ConstantTag::Const_Utf8>();
            break;
        case ConstantTag::Const_i32:
            item.value.emplace<ConstantTag::Const_i32>();
            break;
        case ConstantTag::Const_i64:
            item.value.emplace<ConstantTag::Const_i64>();
            break;
        case ConstantTag::Const_u32:
            item.value.emplace<ConstantTag::Const_u32>();
            break;
        case ConstantTag::Const_u64:
            item.value.emplace<ConstantTag::Const_u64>();
            break;
        case ConstantTag::Const_Type:
            item.value.emplace<ConstantTag::Const_Type>();
            break;
        case ConstantTag::Const_Version:
            item.value.emplace<ConstantTag::Const_Version>();
            break;
        default:
            throw lclib::io::IOException{"Invalid Constant Pool tag"};
    }
    return in >> item.value;
}

lclib::io::DataInputStream &orka::framework::operator>>(lclib::io::DataInputStream & in, orka::framework::Dependency & dep) {
    return in >> dep.modName >> dep.modVersion >> dep.order;
}

lclib::io::DataOutputStream &
orka::framework::operator<<(lclib::io::DataOutputStream & out, const orka::framework::Dependency & dep) {
    return out << dep.modName << dep.modVersion << dep.order;
}

lclib::io::DataInputStream &orka::framework::operator>>(lclib::io::DataInputStream& in, orka::framework::ModuleFile& mod) {
    return in >> mod.magic >> mod.ver >> mod.constants >> mod.mod_name >> mod.mod_version >> mod.dependencies;
}

lclib::io::DataOutputStream &
orka::framework::operator<<(lclib::io::DataOutputStream & out, const orka::framework::ModuleFile & mod) {
    return out << mod.magic << mod.ver << mod.constants << mod.mod_name << mod.mod_version << mod.dependencies;
}

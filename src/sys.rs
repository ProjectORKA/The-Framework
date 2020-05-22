use std::collections::btree_map::BTreeMap;
use std::collections::HashMap;
use std::marker::{PhantomPinned, PhantomData};
use std::path::PathBuf;
use std::pin::Pin;
use std::ffi::c_void;
use std::os::raw::c_char;
use std::ptr::NonNull;

pub struct VTable{
    type_size: usize,
    type_align: usize,
    alloc: Option<unsafe extern"C" fn(usize,usize)->*mut std::ffi::c_void>,
    dealloc: Option<unsafe extern"C" fn(*mut std::ffi::c_void)>,
    construct: Option<unsafe extern"C" fn(*mut std::ffi::c_void)>,
    destroy: Option<unsafe extern"C" fn(*mut std::ffi::c_void)>,
    vfns: [Option<unsafe extern"C" fn(*mut std::ffi::c_void)>;0]
}

mod platform{
    #![allow(non_camel_case_types)]
    #[cfg(windows)]
    mod internal {
        pub type dso_handle = winapi::shared::minwindef::HANDLE;
        pub static OS_NAME: &'static str = "windows";

        pub static VENDORS: [&'static str;1] = ["pc"];

        pub static ABIS: [&'static str;2] = ["gnu","msvc"];

        #[cfg(target_arch="x86_64")]
        pub static ARCHS: [&'static str;1] = ["x86_64"];
        #[cfg(not(any(target_arch="x86_64")))]
        compile_error!("Unsupported architecture, please add an ARCH_NAME");
    }
    #[cfg(unix)]
    mod internal {
        use std::path::{PathBuf, Path};
        use std::os::unix::ffi::OsStrExt;
        use crate::traitext::MaybeNullTerminated;

        pub type dso_handle = *mut std::ffi::c_void;
        #[cfg(target_os = "linux")]
        pub static OS_NAME: &'static str = "linux";
        #[cfg(not(any(target_os = "linux")))]
        compile_error!("Unsupported unix platform, please an an OS_NAME constant");

        pub fn open_dso(path: &PathBuf) -> dso_handle{
            unsafe { libc::dlopen(path.as_os_str().with_null()
                      .as_bytes().as_ptr() as *const _, libc::RTLD_NOW) }
        }

        pub static ABIS: [&'static str;2] = ["gnu","lc"];

        pub static VENDORS: [&'static str;1] = ["pc"];

        #[cfg(target_arch="x86_64")]
        pub static ARCHS: [&'static str;1] = ["x86_64"];
        #[cfg(not(any(target_arch="x86_64")))]
        compile_error!("Unsupported architecture, please add an ARCH_NAME");
    }

    pub use internal::*;
    use std::path::{Path, PathBuf};


    lazy_static::lazy_static!{
            pub static ref targets: Vec<String> = {
                let mut targets_ = Vec::new();
                for arch in &ARCHS{
                    for vendor in &VENDORS{
                        for abi in &ABIS{
                            let tuple = arch.to_string()+"-"+vendor+"-"+OS_NAME+"-"+abi;
                            targets_.push(tuple);
                        }
                    }
                }

                targets_
            };
        }

    pub fn find_from_target<P: AsRef<Path>>(path:  &P)->Vec<PathBuf>{
        targets.iter()
            .map(|s|path.as_ref().join(&s)).collect()
    }
}

pub struct Module<'dso>{
    handle: platform::dso_handle,
    pinned: PhantomPinned,
    data: PhantomData<&'dso c_void>
}



impl<'dso> Module<'dso>{
    pub fn new(p: &PathBuf) -> Pin<Box<Self>>{
        let mut pinned = Box::pin(Self{handle: std::ptr::null_mut(), pinned: PhantomPinned, data: PhantomData });
        let mut ptr = unsafe{pinned.as_mut().get_unchecked_mut()};
        ptr.handle = platform::open_dso(p);
        pinned
    }
}

struct Types<'dso>{
    interfaces: HashMap<String,Interface<'dso>>,
    classes: HashMap<String,Class<'dso>>,
    provided: HashMap<String,Vec<Provided<'dso>>>,
    pinned: PhantomPinned
}

pub struct Interface<'dso>{
    vitems: HashMap<String,usize>,
    def_vtable: Option<&'dso VTable>,
    name: std::string::String
}

#[repr(C)]
pub struct InterfacePtr{
    raw: NonNull<c_void>,
    vtable: NonNull<VTable>
}

pub struct Class<'dso>{
    common_vtable: Option<&'dso VTable>
}

pub struct Provided<'dso>{
    interface: &'dso Interface<'dso>,
    class: &'dso Class<'dso>,
    vtable: &'dso VTable,
    priority: usize
}

#[allow(non_snake_case)]
#[repr(C)]
struct FrameworkCalls{

}

use std::borrow::Cow;
use std::ops::Add;
use std::ffi::{OsStr, CStr};

pub trait MaybeNullTerminated: ToOwned{
    ///
    /// Creates a Cow string that is guaranteed to have a null-character at the end
    /// This allocates if and only if the string does not already
    fn with_null(&self) -> Cow<Self>;

    ///
    /// Promises that self is null terminated
    /// If this guarantee is not satisfied, the behavior is immediately undefined.
    unsafe fn with_null_unchecked(&self)->&Self;
}

impl MaybeNullTerminated for str{
    fn with_null(&self) -> Cow<Self> {
        if let Some(0) = self.bytes().last(){
            Cow::Borrowed(self)
        }else{
            let mut ret = Cow::Borrowed(self);
            ret = ret.add("\0");
            ret
        }
    }

    unsafe fn with_null_unchecked(&self) -> &Self {
        self // At this point it is UB to not end with a nul byte
    }
}

#[cfg(unix)]
impl MaybeNullTerminated for OsStr{
    fn with_null(&self) -> Cow<Self> {
        use std::os::unix::ffi::OsStrExt;
        if let Some(0) = self.as_bytes().last(){
            Cow::Borrowed(self)
        }else{
            let mut os_str = self.to_owned();
            os_str.push("\0");
            Cow::Owned(os_str)
        }
    }

    unsafe fn with_null_unchecked(&self) -> &Self {
        self
    }
}

///
/// Implementation of MaybeNullTerminated for CStr
///
impl MaybeNullTerminated for CStr{
    fn with_null(&self) -> Cow<Self> {
        Cow::Borrowed(self)
    }

    unsafe fn with_null_unchecked(&self) -> &Self {
        self
    }
}
use embedded_io::blocking::{Read, Write};

pub struct Elm327<T: Read + Write> {
    stream: T,
}

impl <T: Read + Write> Elm327<T> {
    pub fn new(stream: T) -> Self {
        Elm327 {
            stream
        }
    }

    fn respond(&mut self, data: &str) {
        self.stream.write(data.as_bytes());
        self.ready();
    }

    fn ready(&mut self) {
        self.stream.write(b"\n>");
    }

    pub fn poll(&mut self) {
        let mut buf = [0u8; 32];
        let req = match self.stream.read(&mut buf) {
            Ok(size) => {
                if size < 3 {
                    return;
                }
                core::str::from_utf8(&buf[0..size]).unwrap()
            }
            Err(_) => return,
        };

        let mode = &req[0..1];
        match mode {
            "AT" => self.handle_at(&req[2..]),
            "01" => self.handle_current(&req[2..]),
            "21" => self.handle_custom(&req[2..]),
            _ => self.ready(),
        }
    }

    fn handle_at(&mut self, req: &str) {
        match req {
            "@1" => self.respond("OBDII Arduino Simulator"),
            "I" => self.respond("VPEE v1.0"),
            "SP0" => self.respond("OK"),
            "DPN" => self.respond("1"),
            "RV" => self.respond("12.5"),
            _ => {},
        }
    }

    fn handle_current(&mut self, req: &str) {
        match req {
            "00" => self.respond("410008180004"),
            "05" => self.respond("41053C"),
            "0C" => self.respond("410C003C"),
            "0D" => self.respond("410D3C"),
            "5E" => self.respond("415E003C"),
            _ => {},
        }
    }

    fn handle_custom(&self, req: &str) {
        match req {
            _ => {},
        }
    }

    fn to_hex(num: usize, buf: &mut [u8]) -> &str {
        let mut num = num;
        let mut i = 0;
        while num > 0 {
            let rem = num % 16;
            buf[i] = match rem {
                0..=9 => (rem + b'0' as usize) as u8,
                _ => (rem - 10 + b'A' as usize) as u8,
            };
            num /= 16;
            i += 1;
        }
        buf.reverse();
        core::str::from_utf8(buf).unwrap()
    }
}

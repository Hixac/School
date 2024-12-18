use tiny_http::{Header, Method, Request, Response, Server, StatusCode};

fn form_response_dynamically(request: Request, bytes: &[u8], content_type: &str, status_code: u16) -> std::io::Result<()> {
	let header = Header::from_bytes("Content-Type", content_type)
		.expect("Shit fucked up header");
	let response = Response::from_data(bytes).with_header(header).with_status_code(StatusCode(status_code));
	
	request.respond(response)
}

fn respond_404(request: Request) -> std::io::Result<()> {
	form_response_dynamically(request, std::include_bytes!("404.html"), "text/html; charset=utf-8", 404)
}

fn respond_500(request: Request) -> std::io::Result<()> {
	form_response_dynamically(request, std::include_bytes!("500.html"), "text/html; charset=utf-8", 500)
}

// I need statically load for perfoming the hotload
fn form_response_statically(request: Request, path: &str, content_type: &str, status_code: u16) -> std::io::Result<()> {
	let file = match std::fs::File::open(path) {
		Ok(f) => f,
		Err(err) => {
			eprintln!("ERROR: file not found: {err}");
			return respond_500(request);
		}
	};

	let header = Header::from_bytes("Content-Type", content_type)
		.expect("Shit fucked up header");
	let response = Response::from_file(file).with_header(header).with_status_code(StatusCode(status_code));

	request.respond(response)
}

fn explore(request: Request) -> std::io::Result<()> {
	match (request.method(), request.url()) {
		(Method::Get, "/main") => {
			form_response_statically(request, "index.html", "text/html; charset=utf-8", 200)
		}
        (Method::Get, "/shop") => {
			form_response_statically(request, "shop.html", "text/html; charset=utf-8", 200)
		}
        (Method::Get, "/help") => {
			form_response_statically(request, "help.html", "text/html; charset=utf-8", 200)
		}
        (Method::Get, "/agreement") => {
			form_response_statically(request, "agreement.html", "text/html; charset=utf-8", 200)
		}
		(Method::Get, "/index.css") => {
			form_response_statically(request, "css/styles.css", "text/css; charset=utf-8", 200)
		}
        (Method::Get, "/perf.jpg") => {
			form_response_dynamically(request, std::include_bytes!("../res/perf.jpg"), "image/jpg; charset=utf-8", 200)
		}
        (Method::Get, "/bol.png") => {
			form_response_dynamically(request, std::include_bytes!("../res/bol.png"), "image/png; charset=utf-8", 200)
		}
		(Method::Get, "/hat.png") => {
			form_response_dynamically(request, std::include_bytes!("../res/hat.png"), "image/png; charset=utf-8", 200)
		}
		(Method::Get, "/404.css") => {
			form_response_dynamically(request, std::include_bytes!("404.css"), "text/css; charset=utf-8", 200)
		}
        (Method::Post, "/API/POST/") => {
            let mut content = String::new();
            request.as_reader().read_to_string(&mut content).unwrap();
            println!("{content}");
            Ok(())
        }
		_ => {
			respond_404(request)
		}
	}
}

fn main() -> std::process::ExitCode {
    let server = Server::http("127.0.0.1:8888").unwrap();
	for request in server.incoming_requests() {
		explore(request).map_err(|err| {
			eprintln!("ERROR: something went wrong exploring: {err}")
		}).ok();
	}

	eprintln!("FATAL: server is down");
	std::process::ExitCode::FAILURE
}

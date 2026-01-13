# ===============================
# Builder stage
# ===============================
FROM rust:1.90-bookworm AS builder

WORKDIR /app

# Cache dependencies
COPY Cargo.toml Cargo.lock ./
RUN mkdir src && echo "fn main() {}" > src/main.rs
RUN cargo build --release
RUN rm -rf src

# Build real application
COPY src ./src
RUN cargo build --release

# ===============================
# Runtime stage
# ===============================
FROM debian:bookworm-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    ca-certificates \
    libssl3 \
    libmariadb3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy binary from builder
COPY --from=builder /app/target/release/rust_rest_api /app/rust_rest_api

# Expose API port
EXPOSE 5000

# Run binary
CMD ["./rust_rest_api"]


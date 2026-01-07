# =========================
# Build stage
# =========================
FROM debian:bookworm AS builder

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    libboost-all-dev \
    libmariadb-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source
COPY . .

# -------------------------
# Fetch served
# -------------------------
RUN rm -rf lib/served && \
    git clone https://github.com/evanugarte/served.git lib/served

# -------------------------
# Fetch SimpleJSON
# -------------------------
RUN mkdir -p lib/SimpleJSON && \
    curl -fsSL https://raw.githubusercontent.com/nbsdx/SimpleJSON/master/json.hpp \
    -o lib/SimpleJSON/json.hpp

# -------------------------
# Build
# -------------------------
RUN mkdir -p bin && make

# =========================
# Runtime stage
# =========================
FROM alpine:3.19

# Install runtime dependencies only
RUN apk add --no-cache \
    libstdc++ \
    boost-system \
    mariadb-connector-c

# Create app directory
WORKDIR /app

# Copy binary from builder
COPY --from=builder /app/bin/rest_api ./rest_api

# Expose your API port
EXPOSE 5000

# Run the server
CMD ["./rest_api"]


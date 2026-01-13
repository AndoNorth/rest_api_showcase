use axum::Router;
use sqlx::MySqlPool;
use std::net::SocketAddr;
use tokio::net::TcpListener;

use crate::handlers;

#[derive(Clone)]
pub struct AppState {
    pub db: MySqlPool,
}

pub async fn run(db: MySqlPool) -> Result<(), Box<dyn std::error::Error>> {
    let state = AppState { db };

    let app = Router::new()
        .nest("/videos", handlers::videos::routes())
        .with_state(state);

    let addr = SocketAddr::from(([0, 0, 0, 0], 5000));
    tracing::info!("Listening on {}", addr);

    let listener = TcpListener::bind(addr).await?;
    axum::serve(listener, app).await?;

    Ok(())
}

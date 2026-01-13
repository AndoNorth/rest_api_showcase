use axum::{
    extract::{Path, State},
    http::StatusCode,
    response::IntoResponse,
    routing::{delete, get, post, put},
    Json, Router,
};
use serde::{Deserialize, Serialize};
use sqlx::{FromRow, MySqlPool};
use std::net::SocketAddr;
use tracing_subscriber;
use tokio::net::TcpListener;
use std::time::Duration;
use tokio::time::sleep;

#[derive(Serialize, FromRow)]
struct Video {
    id: i32,
    name: String,
    likes: i32,
    views: i32,
}

#[derive(Deserialize)]
struct VideoInput {
    name: Option<String>,
    likes: Option<i32>,
    views: Option<i32>,
}

#[derive(Clone)]
struct AppState {
    db: MySqlPool,
}


#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    tracing_subscriber::fmt::init();


    let database_url = "mysql://rest_user:password123@mysql/rest_db_rust";
    println!("Connecting to mysql with {}", database_url);

    let db = connect_db(database_url).await; 
    let state = AppState { db };

    let app = Router::new()
        .route("/videos", get(get_all_videos))
        .route(
            "/videos/:id",
            get(get_video)
                .put(put_video)
                .post(post_video)
                .delete(delete_video),
        )
        .with_state(state);

    let addr = SocketAddr::from(([0, 0, 0, 0], 5000));
    println!("Rust REST API listening on {}", addr);

    let listener = TcpListener::bind(addr).await?;
    axum::serve(listener, app).await?;

    Ok(())
}

async fn connect_db(url: &str) -> MySqlPool {
    loop {
        match MySqlPool::connect(url).await {
            Ok(pool) => return pool,
            Err(e) => {
                eprintln!("DB not ready yet: {}. Retrying in 2s...", e);
                sleep(Duration::from_secs(2)).await;
            }
        }
    }
}

// handlers
async fn get_all_videos(
    State(state): State<AppState>,
) -> impl IntoResponse {
    let result = sqlx::query_as::<_, Video>(
        "SELECT id, name, likes, views FROM videos",
    )
    .fetch_all(&state.db)
    .await;

    match result {
        Ok(videos) => (StatusCode::OK, Json(videos)).into_response(),
        Err(err) => {
            eprintln!("DB error: {}", err);
            StatusCode::INTERNAL_SERVER_ERROR.into_response()
        }
    }
}

async fn get_video(
    State(state): State<AppState>,
    Path(id): Path<i32>,
) -> impl IntoResponse {
    let result = sqlx::query_as::<_, Video>(
        "SELECT id, name, likes, views FROM videos WHERE id = ?",
    )
    .bind(id)
    .fetch_optional(&state.db)
    .await;

    match result {
        Ok(Some(video)) => (StatusCode::OK, Json(video)).into_response(),
        Ok(None) => StatusCode::NOT_FOUND.into_response(),
        Err(err) => {
            eprintln!("DB error: {}", err);
            StatusCode::INTERNAL_SERVER_ERROR.into_response()
        }
    }
}

async fn put_video(
    State(state): State<AppState>,
    Path(id): Path<i32>,
    Json(body): Json<VideoInput>,
) -> impl IntoResponse {
    let result = sqlx::query(
        r#"
        REPLACE INTO videos (id, name, likes, views)
        VALUES (?, ?, ?, ?)
        "#,
    )
    .bind(id)
    .bind(body.name.unwrap_or_default())
    .bind(body.likes.unwrap_or(0))
    .bind(body.views.unwrap_or(0))
    .execute(&state.db)
    .await;

    match result {
        Ok(_) => StatusCode::OK.into_response(),
        Err(err) => {
            eprintln!("DB error: {}", err);
            StatusCode::INTERNAL_SERVER_ERROR.into_response()
        }
    }
}

async fn post_video(
    State(state): State<AppState>,
    Path(id): Path<i32>,
    Json(body): Json<VideoInput>,
) -> impl IntoResponse {
    let mut fields = Vec::new();
    let mut values = Vec::new();

    if let Some(name) = body.name {
        fields.push("name = ?");
        values.push(name);
    }
    if let Some(likes) = body.likes {
        fields.push("likes = ?");
        values.push(likes.to_string());
    }
    if let Some(views) = body.views {
        fields.push("views = ?");
        values.push(views.to_string());
    }

    if fields.is_empty() {
        return StatusCode::BAD_REQUEST.into_response();
    }

    let query = format!(
        "UPDATE videos SET {} WHERE id = ?",
        fields.join(", ")
    );

    let mut q = sqlx::query(&query);
    for value in values {
        q = q.bind(value);
    }
    q = q.bind(id);

    match q.execute(&state.db).await {
        Ok(_) => StatusCode::OK.into_response(),
        Err(err) => {
            eprintln!("DB error: {}", err);
            StatusCode::INTERNAL_SERVER_ERROR.into_response()
        }
    }
}

async fn delete_video(
    State(state): State<AppState>,
    Path(id): Path<i32>,
) -> impl IntoResponse {
    let result = sqlx::query("DELETE FROM videos WHERE id = ?")
        .bind(id)
        .execute(&state.db)
        .await;

    match result {
        Ok(_) => StatusCode::OK.into_response(),
        Err(err) => {
            eprintln!("DB error: {}", err);
            StatusCode::INTERNAL_SERVER_ERROR.into_response()
        }
    }
}


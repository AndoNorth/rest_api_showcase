use axum::{
    extract::{Path, State},
    http::StatusCode,
    response::IntoResponse,
    routing::get,
    Json, Router,
};

use crate::app::AppState;
use crate::models::video::{Video, VideoInput};

pub fn routes() -> Router<AppState> {
    Router::new().route("/", get(get_all_videos)).route(
        "/:id",
        get(get_video)
            .put(put_video)
            .post(post_video)
            .delete(delete_video),
    )
}

async fn get_all_videos(State(state): State<AppState>) -> impl IntoResponse {
    let result = sqlx::query_as::<_, Video>("SELECT id, name, likes, views FROM videos")
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

async fn get_video(State(state): State<AppState>, Path(id): Path<i32>) -> impl IntoResponse {
    let result =
        sqlx::query_as::<_, Video>("SELECT id, name, likes, views FROM videos WHERE id = ?")
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

    let query = format!("UPDATE videos SET {} WHERE id = ?", fields.join(", "));

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

async fn delete_video(State(state): State<AppState>, Path(id): Path<i32>) -> impl IntoResponse {
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

use serde::{Deserialize, Serialize};
use sqlx::FromRow;

#[derive(Serialize, FromRow)]
pub struct Video {
    id: i32,
    name: String,
    likes: i32,
    views: i32,
}

#[derive(Deserialize)]
pub struct VideoInput {
    pub name: Option<String>,
    pub likes: Option<i32>,
    pub views: Option<i32>,
}

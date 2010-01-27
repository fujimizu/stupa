struct SearchResult {
  1: string name
  2: double point
}

service Stupa {
  void add_document(1: string document_id, 2: list<string> features),
  void delete_document(1: string document_id),
  i64 size(),
  list<SearchResult> search(1: i64 max, 2: list<string> query)
}

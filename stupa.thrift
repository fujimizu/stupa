struct SearchResult {
  1: string name
  2: double point
}

service StupaThrift {
  void add_document(1: string document_id, 2: list<string> features),
  void delete_document(1: string document_id),
  i64 size(),
  list<SearchResult> search_by_document(1: i64 max, 2: list<string> query),
  list<SearchResult> search_by_feature(1: i64 max, 2: list<string> query),
  bool save(1: string filename),
  bool load(1: string filename)
}

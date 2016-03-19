class AddIndexToLinks < ActiveRecord::Migration
  def change
	add_index :links, [:id1, :id2, :linktype], unique: true
  end
end

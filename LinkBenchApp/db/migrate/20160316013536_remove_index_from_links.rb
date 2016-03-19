class RemoveIndexFromLinks < ActiveRecord::Migration
  def change
	remove_index :links, [:id1, :id2, :linktype]
  end
end
